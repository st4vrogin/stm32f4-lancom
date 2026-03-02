/*
 * ticker.c
 *
 *  Created on: Feb 24, 2026
 *      Author: Siel Tre
 */
#include "common_data.h"
#include "FreeRTOS.h"
#include "common_tasks.h"
#include "cmsis_os2.h"
#include "common_data.h"
#include "event_groups.h"
//#include "common_data.h"


/********************************************************************
 * Module variables
 */
static EventGroupHandle_t xEventTickerGroupHandle;
static StaticEventGroup_t TickerEventGroup;
extern IWDG_HandleTypeDef hiwdg;
//*******************************************************************

/********************************************************************
 * Private definitions
 */
#define BIT_0					( 1 << 0 )
#define BIT_1					( 1 << 1 )
#define EVENT_TICK_1MS			BIT_0
#define EVENT_RESET				BIT_1
#define THIS_STACK_SIZE			160
//*******************************************************************

/********************************************************************
 * Forward declarations
 */
void	manage_over_mem_requests(void);
__WEAK void		ApplicationTicker(void)
{
}
//*******************************************************************

const osThreadAttr_t tickerTask_attributes = {
  .name = "Ticker",
  .stack_size = THIS_STACK_SIZE,
  .priority = (osPriority_t) osPriorityHigh
};


/**
 * @brief Application Ticker hook. Viene richiamata da FreeRTOS ogni volta che scatta il tick
 *
 * Attenzione: Si trova all'interno di un ISR, quindi deve essere veloce
 */
void vApplicationTickHook( void )
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	over_mem.sys_flag.bit.tick_1mS = 1;
	xEventGroupSetBitsFromISR(
		xEventTickerGroupHandle,   /* The event group being updated. */
		EVENT_TICK_1MS, /* The bits being set. */
		&xHigherPriorityTaskWoken );
}

/**
 * @brief Routine di inizializzazione del task ticker
 *
 */
void initTickerTask(void)
{
	xEventTickerGroupHandle = xEventGroupCreateStatic( &TickerEventGroup );
}

/**
 * @brief Il task di gestione dei timer
 *
 */
void tickerTask(void *pvParameters)
{
	const BaseType_t xClearBits = pdTRUE, xWaitAll = pdFALSE;
	EventBits_t uxBits;
	for (;;)
	{
		uxBits = xEventGroupWaitBits(xEventTickerGroupHandle, EVENT_TICK_1MS | EVENT_RESET, xClearBits, xWaitAll, portMAX_DELAY);
		if((uxBits & EVENT_RESET) == EVENT_RESET)
		{
			HAL_NVIC_SystemReset();
		}
		if((uxBits & EVENT_TICK_1MS) == EVENT_TICK_1MS)
		{
#ifndef NOWATCHDOG
			HAL_IWDG_Refresh(&hiwdg);
#endif
			over_mem.sys_flag.bit.tick_1mS = 0;
			ApplicationTicker();
			manage_over_mem_requests();
		}
	}
}

/**
 * @brief Routine di creazione del thread FreeRTOS (ricezione da USB)
 *
 * Va richiamata in main e riportata in tasks.h
 */
osThreadId_t createTask_Ticker(void* arg)
{
	initTickerTask();
	return osThreadNew(tickerTask, arg, &tickerTask_attributes);
}

/*******************************************************
	manage_over_mem_requests
	Controlli di over_mem ad ogni giro di schedulazione
********************************************************/
void	manage_over_mem_requests(void)
{
	// 1. gestisce richiesta reset
	if (over_mem.sys_flag.bit.req_reset)
	{
		over_mem.sys_flag.bit.runMain = 0;
		over_mem.sys_flag.bit.req_reset = 0;
		xEventGroupSetBits(xEventTickerGroupHandle, EVENT_RESET);
		return;
	}

	// 2. Verifica richiesta di jmp in Loader ritardato
	if (over_mem.sys_flag.bit.req_jmp_loader_rit == 0)
	{
		systmr_b_5ms.ritardo_reset = 0;
	}
	else
	{
		// Richiesta di reset ritardato in corso
		if (systmr_b_5ms.ritardo_reset > over_mem.timeout_ric_reset_5mS)
		{
			over_mem.timeout_ric_reset_5mS = 0;
			over_mem.sys_flag.bit.req_jmp_loader_rit = 0;
			over_mem.sys_flag.bit.stay_loader = 1;
			over_mem.sys_flag.bit.req_reset = 1;
			return;
		}
	}

	// 3. Verifica richiesta di JMP IN MAIN RITARDATO
	if (over_mem.sys_flag.bit.req_jmp_main_rit == 0)
	{
		systmr_b_5ms.ritardo_jmp_main = 0;
	}
	else
	{
		// Richiesta di jmp in main ritardato in corso
		if (systmr_b_5ms.ritardo_jmp_main > over_mem.timeout_ric_reset_5mS)
		{
			// Eseguiamo un reset azzerando il bit di "resta in Loader"
			over_mem.timeout_ric_reset_5mS = 0;
			over_mem.sys_flag.bit.req_jmp_main_rit = 0;
			over_mem.sys_flag.bit.stay_loader = 0;
			over_mem.sys_flag.bit.req_reset = 1;
		}
	}

}

/*
 * gestio.c
 *
 *  Created on: Feb 11, 2026
 *      Author: Siel Tre
 */
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "common_tasks.h"
#include "common_data.h"
#include "Protocol/SielType.h"
#include "Protocol/Codop.h"
#include <string.h>
#include "semphr.h"

/********************************************************************
 * Private module declarations
 */
void 	gesComRtosTask(void *pvParameters);
void	initTaskGesCom(void);
uint16_t	GesCodOp(uint8_t codop);
void	gcWriteResponse(SielMessage_t *);
//*******************************************************************

/********************************************************************
 * Private definitions
 */
#define THIS_STACK_SIZE			2 * DEFAULT_STACK_SIZE
//*******************************************************************

/********************************************************************
 * Module variables
 */
static StaticTask_t thread_gesCom_tcb;	//Thread Control Block
static uint8_t gescom_stack[THIS_STACK_SIZE];
SemaphoreHandle_t GescomMutex;

const cod_op_func_t cod_op_kernel[] =
{
	{   cod_op_mancante_k },		// 0x00
	{   cod_op_02 },				// 0x02
	{   cod_op_04 },				// 0x04
	{   cod_op_06 },				// 0x06
	{   cod_op_08 },				// 0x08
	{   cod_op_0A },				// 0x0A
	{   cod_op_0C },				// 0x0C
	{   cod_op_0E },				// 0x0E
	{   cod_op_10 },				// 0x10
	{   cod_op_mancante_k },		// 0x12
	{   /* cod_op_14 */ cod_op_mancante_k },				// 0x14
	{   cod_op_16 },				// 0x16
	{   cod_op_mancante_k },		// 0x18
	{   cod_op_mancante_k },		// 0x1A
	{   cod_op_mancante_k },		// 0x1C
	{   cod_op_mancante_k },		// 0x1E
};
//*******************************************************************

/*****************************************************************
	COD_OP MANCANTE:
	codice operativo non definito
******************************************************************/
uint16_t cod_op_mancante_k(void)
{
	ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_COP_NOT_SUP;
	return(0);
}

const osThreadAttr_t gesComTask_attributes = {
  .name = "GesCom",
  .stack_mem = gescom_stack,
  .stack_size = THIS_STACK_SIZE,
  .priority = (osPriority_t) osPriorityHigh,
  .cb_mem = &thread_gesCom_tcb,
  .cb_size = sizeof(thread_gesCom_tcb)
};


/**
 * @brief Routine di creazione del thread FreeRTOS
 *
 * Va richiamata in main e riportata in tasks.h
 */
osThreadId_t createTask_Gescom(void* arg)
{
	return osThreadNew(gesComRtosTask, arg, &gesComTask_attributes);
}


/**
 * @brief Routine principale di gestione degli I/O.
 *
 * Si occupa della gestione dei comandi in arrivo dalle diverse periferiche
 */
void gesComRtosTask(void* argument)
{
	uint16_t rispLen;
	uint8_t tmp[ADDR_LEN];
	SielMessage_t msg;
	uint8_t prio;
	initTaskGesCom();
	printf("gescom RX Ready");
	for(;;)
	{
		if (osMessageQueueGet(gescomRxQueue, &msg, &prio, osWaitForever) == osOK)
		{
			// Qui il task è stato svegliato dalla ricezione di un messaggio da parte di un altro task
			if (msg.header.len == 0 || msg.netto == NULL)
				continue;

			//Se non sono il destinatario, soprassiedo
			if (msg.header.to != ENTITY_GESCOM)
				continue;

			//Se la scheda non è destinataria del messaggio, soprassiedo
			if (memcmp(msg.dest, over_mem.AddressScheda, ADDR_LEN) || msg.gestore != 0x20)
				continue;

			printf("GESCOM Messaggio da %hu (%hu%hu%hu) a %hu (%hu%hu%hu) cod_op %hu\n", msg.header.from, msg.mitt[0], msg.mitt[1], msg.mitt[2], msg.header.to, msg.dest[0], msg.dest[1], msg.dest[2], msg.cod_op);
			//Memorizzo su ComDatiGescom la richiesta immagazzinata in msg
			ComDatiGescom.buf.cmd.gestore = msg.gestore;
			memcpy(ComDatiGescom.buf.cmd.dest, msg.mitt, ADDR_LEN);
			memcpy(ComDatiGescom.buf.cmd.mitt, msg.dest, ADDR_LEN);
			ComDatiGescom.buf.cmd.cod_op = msg.cod_op;
			ComDatiGescom.buf.cmd.flag = msg.flag;
			memcpy(ComDatiGescom.buf.cmd.dati, msg.netto, msg.header.len);

			//Eseguo il codice operativo
			if (pApplCodop != NULL && ComDatiGescom.buf.cmd.cod_op >= 0x20)
				rispLen = pApplCodop(ComDatiGescom.buf.cmd.cod_op);
			else
				rispLen = GesCodOp(ComDatiGescom.buf.cmd.cod_op);

			//Preparo la risposta
			msg.header.len = rispLen;
			tmp[0] = msg.header.from;
			msg.header.from = msg.header.to;
			msg.header.to = tmp[0];
			memcpy(tmp, msg.mitt, ADDR_LEN);
			memcpy(msg.mitt, msg.dest, ADDR_LEN);
			memcpy(msg.dest, tmp, ADDR_LEN);
			msg.cod_op = ComDatiGescom.buf.cmd.cod_op + 1;
			msg.flag = ComDatiGescom.buf.risp.flag;
			msg.netto = ComDatiGescom.buf.risp.dati;
			printf("GESCOM Sending response to %hu cod_op %hu, len %u\n", msg.header.to, msg.cod_op, rispLen);
			//Scrivo la risposta
			gcWriteResponse(&msg);

		}
	}

}

/**
 * @brief Routine di inizializzazione del task
 *
 * Fa le inizializzazioni alla partenza del thread
 */
void	initTaskGesCom(void)
{
	gescomRxQueue = osMessageQueueNew(USB_QUEUE_LENGTH, sizeof(SielMessage_t), NULL);
	ptrBufCom = &ComDatiGescom;
	// TODO: Temporaneamente inizializzo l'indirizzo della scheda
	if(over_mem.check_word != VALID_W16)
	{
		memset(&over_mem, 0, sizeof(over_mem_t));
		over_mem.check_word = VALID_W16;
		over_mem.AddressScheda[0] = BROADCAST_0;
		over_mem.AddressScheda[1] = BROADCAST_1;
		over_mem.AddressScheda[2] = BROADCAST_2;
		over_mem.sys_flag.bit.runMain = 1;
	}
	GescomMutex = xSemaphoreCreateMutex();
}

uint16_t	GesCodOp(uint8_t codop)
{
	uint8_t idx = codop >> 1;
	if (idx >= (uint8_t)(sizeof(cod_op_kernel)/sizeof(cod_op_func_t)))
	{
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_COP_NOT_SUP;
		return 0;
	}
	return cod_op_kernel[codop >> 1].func();
}

void gcWriteResponse(SielMessage_t *resp)
{
	if(resp == NULL)
		return;
	switch(resp->header.to)
	{
		case ENTITY_USB:
			osMessageQueuePut(usbTxQueue, resp, 1, osWaitForever);
			break;
		case ENTITY_LAN_GES_0:
		case ENTITY_LAN_GES_1:
		case ENTITY_LAN_GES_2:
			osMessageQueuePut(lgcQueueTx[resp->header.to - ENTITY_LAN_GES_0], resp, 1, osWaitForever);
			break;
		default:
			break;
	}
}



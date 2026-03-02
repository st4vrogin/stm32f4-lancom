/*
 * usbcomm.c
 *
 *  Created on: Feb 12, 2026
 *      Author: Siel Tre
 */
#include "data.h"
#include "appl_tasks.h"
#include "queue.h"
#include "usbd_def.h"
#include "usbd_cdc_if.h"
#include "Protocol/SielPtcl.h"
#include "mem_siel_rtos.h"
#include "common_data.h"
#include "cmsis_os2.h"

/********************************************************************
 * Private module declarations
 */
void UsbCommRxTask(void* argument);
void UsbCommTxTask(void* argument);
void initUsbTasks(void);
//*******************************************************************

/********************************************************************
 * Private definitions
 */
#define THIS_STACK_SIZE			DEFAULT_STACK_SIZE
//*******************************************************************

/********************************************************************
 * Module variables
 */
static StaticTask_t thread_usbComm_tcb[2];   //Thread Control Block
static uint8_t usbcomm_stack_rx[THIS_STACK_SIZE];
static uint8_t usbcomm_stack_tx[THIS_STACK_SIZE];
static uint8_t* usbTempBuffer;
static StaticStreamBuffer_t usbStreamBufferStruct;
static uint16_t sizeTempBuffer;
//*******************************************************************

const osThreadAttr_t usbCommTaskRx_attributes = {
  .name = "UsbCommRx",
  .stack_mem = usbcomm_stack_rx,
  .stack_size = THIS_STACK_SIZE,
  .priority = (osPriority_t) osPriorityHigh,
  .cb_mem = &thread_usbComm_tcb[0],
  .cb_size = sizeof(thread_usbComm_tcb[0])
};

const osThreadAttr_t usbCommTaskTx_attributes = {
  .name = "UsbCommTx",
  .stack_mem = usbcomm_stack_tx,
  .stack_size = THIS_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal,
  .cb_mem = &thread_usbComm_tcb[1],
  .cb_size = sizeof(thread_usbComm_tcb[1])
};


/**
 * @brief Routine di creazione del thread FreeRTOS (ricezione da USB)
 *
 * Va richiamata in main e riportata in tasks.h
 */
osThreadId_t createTask_UsbCommRx(void* arg)
{
	sizeTempBuffer = 0;
	usbTempBuffer = NULL;
	initUsbTasks();
	return osThreadNew(UsbCommRxTask, arg, &usbCommTaskRx_attributes);
}

/**
 * @brief Routine di creazione del thread FreeRTOS (trasmissione USB)
 *
 * Va richiamata in main e riportata in tasks.h
 */
osThreadId_t createTask_UsbCommTx(void* arg)
{
	//sizeTempBuffer = 0;
	//usbTempBuffer = NULL;
	return osThreadNew(UsbCommTxTask, arg, &usbCommTaskTx_attributes);
}

void UsbCommRxTask(void* argument)
{
	uint16_t		i = 0;
	uint8_t 		msgBuffer[300];
	uint8_t 		rxByte;
	SielMessage_t	req;
	e_InPlaceSielProtocolStatus sps_status = SPS_WAIT_STX;
	for(;;)
	{
		if (xStreamBufferReceive(usbStreamBuffer, &rxByte, 1, portMAX_DELAY) > 0)
		{
			// Qui il task è stato svegliato dall'ISR

			// Boundary Check sul buffer
			// Macchina a stati finiti per il destuffing
			sps_status = spDestuffBufferInPlace(rxByte, sps_status, msgBuffer, &i);

			if (sps_status == SPS_END)
			{
				//In posizione i - 1 trovo la checksum quindi devo calcolarla tra 0 e i - 2 (lunghezza utile proprio pari a i -1)
				rxByte = calcolo_chk8(&msgBuffer[1], i - 2);
				if(rxByte == msgBuffer[i - 1])
				{
					req.header.from = ENTITY_USB;
					req.header.to = ENTITY_GESCOM;
					memcpy(&req.gestore, msgBuffer, 9);
					//I dati netti sono i - 1 - 9;
					req.netto = &msgBuffer[9];
					req.header.len = i - 1 - 9;
					osMessageQueuePut(gescomRxQueue, &req, 1, osWaitForever);
				}
				sps_status = SPS_WAIT_STX;
			}
		}
	}

}

void UsbCommTxTask(void* argument)
{
	SielMessage_t msg;
	uint8_t msgBuffer[300];
	uint16_t bufLen;
	uint8_t cksum;
	uint8_t* tmp;
	for(;;)
	{
		if (xQueueReceive(usbTxQueue, &msg, portMAX_DELAY) == pdTRUE)
		{
			// Qui il task è stato svegliato da un altro task

			//Se non ci sono dati, soprassiedo
			if (msg.header.len == 0 || msg.netto == NULL)
				continue;

			//Se non sono il destinatario, soprassiedo
			if(msg.header.to != ENTITY_BROADCAST && msg.header.to != ENTITY_USB)
				continue;


			bufLen = 300; cksum = 0xFF;
			//Calcolo della checksum
			cksum = CalcolaChecksum(&msg);
			//Aggiungo la checksum
			msg.netto[msg.header.len++] = cksum;
			//Stuffing
			tmp = spStuffMessage(&msg, msgBuffer, &bufLen);
			//Libero il netto.
			//msrFree(msg.netto);
			if(tmp != NULL && bufLen > 0)
			{
				while(CDC_Transmit_FS(msgBuffer, bufLen) == USBD_BUSY)
				{
					vTaskDelay(1);
				}
			}
		}
	}
}

void initUsbTasks(void)
{
	usbTxQueue = osMessageQueueNew(USB_QUEUE_LENGTH, sizeof(SielMessage_t), NULL);
	usbStreamBuffer = xStreamBufferCreateStatic(sizeof(usbExchangeBuffer), 1, usbExchangeBuffer, &usbStreamBufferStruct);
}


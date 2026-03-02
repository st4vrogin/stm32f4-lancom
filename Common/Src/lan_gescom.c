/*
 * lan_gescom.c
 *
 *  Created on: Feb 20, 2026
 *      Author: Siel Tre
 */
#include "common_data.h"
#include "FreeRTOS_sockets.h"
#include "mem_siel_rtos.h"
#include "Protocol/SielPtcl.h"
#include "common_tasks.h"
#include "FreeRTOS_IP.h"
#include "lan_init.h"

#define		RX_TASK_STACK_SIZE				1512		//Dimensione dello stack del task di ricezione
#define		TX_TASK_STACK_SIZE				DEFAULT_STACK_SIZE		//Dimensione dello stack del task di trasmissione

void prvHandleGescomConnection(uint8_t index);

void vTCPServerGescomRxTask(void *pvParameters);
void vTCPServerGescomTxTask(void* argument);
void GescomServerDataProcessTask(void *pvParameters);
void initLanGescomTasks(void);

static Socket_t GescomClients[NUM_GES_COM_LAN];
static uint8_t taskStatus[NUM_GES_COM_LAN];
static BaseType_t gcInitDone = pdFALSE;

const osThreadAttr_t TCPServerGescomRxTask_attributes[NUM_GES_COM_LAN] = {
	{
	  .name = "TcpGesRx0",
	  .stack_size = RX_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "TcpGesRx1",
	  .stack_size = RX_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},{
	  .name = "TcpGesRx2",
	  .stack_size = RX_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
};

const osThreadAttr_t TCPServerGescomTxTask_attributes[NUM_GES_COM_LAN] = {
	{
	  .name = "TcpGesTx0",
	  .stack_size = TX_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "TcpGesTx1",
	  .stack_size = TX_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},{
	  .name = "TcpGesTx2",
	  .stack_size = TX_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
};


osThreadId_t 	createTask_LanGescomRx(void* arg)
{
	uint32_t index = (uint32_t)arg;
	if(gcInitDone == pdFALSE)
		initLanGescomTasks();
	return osThreadNew(vTCPServerGescomRxTask, arg, &TCPServerGescomRxTask_attributes[index]);
}

osThreadId_t 	createTask_LanGescomTx(void* arg)
{
	uint32_t index = (uint32_t)arg;
	if(gcInitDone == pdFALSE)
		initLanGescomTasks();
	return osThreadNew(vTCPServerGescomTxTask, arg, &TCPServerGescomTxTask_attributes[index]);
}



void vTCPServerGescomRxTask(void *pvParameters)
{
	uint32_t index = (uint32_t)pvParameters;
	Socket_t xListeningSocket;
	BaseType_t xReuseSocket = pdTRUE;
	struct freertos_sockaddr xBindAddress;
	struct freertos_sockaddr xClientAddress;
	socklen_t xSize = sizeof(xClientAddress);
	for(;;)
	{
		if(taskStatus[index] == TASK_RUNNING)
		{
			xListeningSocket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP);
			configASSERT(xListeningSocket != FREERTOS_INVALID_SOCKET);
			FreeRTOS_setsockopt(xListeningSocket, 0, FREERTOS_SO_REUSE_LISTEN_SOCKET, ( void * ) &xReuseSocket, sizeof( xReuseSocket ));
			xBindAddress.sin_port = FreeRTOS_htons(commonParametri.porta_ges_com[index]);
			FreeRTOS_bind(xListeningSocket, &xBindAddress, sizeof(xBindAddress));
			FreeRTOS_listen(xListeningSocket, 1);
			printf("GESCOM %lu socket listen\n", index);
			// 4. Accept: il task si blocca finché un client non si connette
			GescomClients[index] = FreeRTOS_accept(xListeningSocket, &xClientAddress, &xSize);

			if (GescomClients[index] != FREERTOS_INVALID_SOCKET) {

				printf("GESCOM %lu socket ACCEPT\n", index);
				//TickType_t xTimeOut = pdMS_TO_TICKS(5000);
				//FreeRTOS_setsockopt(GescomClients[index], 0, FREERTOS_SO_RCVTIMEO, &xTimeOut, sizeof(xTimeOut));

				// 5. Gestione della sessione
				prvHandleGescomConnection((uint8_t)index);

				// 6. Chiusura (Graceful shutdown)
				FreeRTOS_shutdown(GescomClients[index], FREERTOS_SHUT_RDWR);
				while(FreeRTOS_recv(GescomClients[index], NULL, 0, 0) >= 0) {
					// Svuota i buffer residui prima di chiudere definitivamente
				}
				FreeRTOS_closesocket(GescomClients[index]);
				GescomClients[index] = FREERTOS_INVALID_SOCKET;
			}
		}
		else
			osDelay(100);
    }
}


void prvHandleGescomConnection(uint8_t idx)
{
    uint8_t ucPacketBuffer[MAX_COM_DATA_UART], cksum;
    uint8_t *tmp;
    uint8_t prio;
    uint16_t bufLen;
    int32_t lBytesRead;
	SielMessage_t	msg;
	BaseType_t waitForGescomResponse = pdFALSE;

    for(;;)
    {
    	int32_t lBytesToSend = 0;
    	uint8_t *pucZeroCopyRxBuffPtr = NULL;
        // Ricezione dati dal TCP stream
        lBytesRead = FreeRTOS_recv(GescomClients[idx], &pucZeroCopyRxBuffPtr, ipconfigTCP_MSS, FREERTOS_ZERO_COPY);

        // Gestione errori sui socket
		if (lBytesRead < 0)
		{
			if (pucZeroCopyRxBuffPtr != NULL)
				FreeRTOS_ReleaseTCPPayloadBuffer(GescomClients[idx], pucZeroCopyRxBuffPtr, lBytesRead);
			if(lBytesRead != -pdFREERTOS_ERRNO_EAGAIN)
			{
				printf("GESCOM %hu socket ERROR: %ld\n", idx, lBytesRead);
				break;
			}
			else
				continue;
		}

        if( pucZeroCopyRxBuffPtr != NULL )
        {
        	lBytesToSend = lBytesRead;
        	if(lBytesRead > 300)
        		lBytesToSend = 300;
        	e_InPlaceSielProtocolStatus sps_status = SPS_WAIT_STX;
        	uint16_t i = 0, j = 0;
        	while (j < lBytesToSend && sps_status != SPS_END)
        		sps_status = spDestuffBufferInPlace(pucZeroCopyRxBuffPtr[j++], sps_status, ucPacketBuffer, &i);
        	FreeRTOS_ReleaseTCPPayloadBuffer(GescomClients[idx], pucZeroCopyRxBuffPtr, lBytesRead);
        	lBytesToSend = i;
        	if (sps_status == SPS_END && i > 10)
			{
				//In posizione i - 1 trovo la checksum quindi devo calcolarla tra 0 e i - 2 (lunghezza utile proprio pari a i -1)
				uint8_t cksByte = calcolo_chk8(&ucPacketBuffer[1], i - 2);
				if(cksByte == ucPacketBuffer[i - 1])
				{
					msg.header.from = ENTITY_LAN_GES_0 + (uint8_t)idx;
					msg.header.to = ENTITY_GESCOM;
					memcpy(&msg.gestore, ucPacketBuffer, 9);
					//I dati netti sono i - 1 - 9;
					msg.netto = &ucPacketBuffer[9];
					msg.header.len = i - 1 - 9;
					osMessageQueuePut(gescomRxQueue, &msg, 1, osWaitForever);
					waitForGescomResponse = pdTRUE;
				}
			}
    	}
        if (waitForGescomResponse == pdTRUE)
        {
        	if (osMessageQueueGet(lgcQueueTx[idx], &msg, &prio, osWaitForever ) == osOK)
			{
				// Qui il task è stato svegliato da un altro task

				//Se non ci sono dati, soprassiedo
				if (msg.header.len == 0 || msg.netto == NULL)
					continue;

				//Se non sono il destinatario, soprassiedo
				if(msg.header.to != ENTITY_BROADCAST && msg.header.to != (uint8_t)(ENTITY_LAN_GES_0 + idx))
					continue;

				bufLen = MAX_COM_DATA_UART; cksum = 0xFF;
				//Calcolo della checksum
				cksum = CalcolaChecksum(&msg);
				//Aggiungo la checksum
				msg.netto[msg.header.len++] = cksum;
				//Stuffing
				tmp = spStuffMessage(&msg, ucPacketBuffer, &bufLen);
				//Libero il netto.
				//msrFree(msg.netto);

				if(tmp != NULL && bufLen > 0 && GescomClients[idx] != FREERTOS_INVALID_SOCKET)
				{
					FreeRTOS_send(GescomClients[idx], ucPacketBuffer, bufLen, /*FREERTOS_ZERO_COPY */ 0);
				}
			}
        	waitForGescomResponse = pdFALSE;
        }
    }
}

/*
void vTCPServerGescomTxTask(void* argument)
{
	uint32_t index = (uint32_t)argument;
	SielMessage_t msg;
	uint8_t msgBuffer[300];
	uint16_t bufLen;
	uint8_t cksum;
	uint8_t* tmp;
	for(;;)
	{
		if (xQueueReceive(lgcQueueTx[index], &msg, portMAX_DELAY) == pdTRUE)
		{
			// Qui il task è stato svegliato da un altro task

			//Se non ci sono dati, soprassiedo
			if (msg.header.len == 0 || msg.netto == NULL)
				continue;

			//Se non sono il destinatario, soprassiedo
			if(msg.header.to != ENTITY_BROADCAST && msg.header.to != (ENTITY_LAN_GES_0 + (uint8_t)index))
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

			if(tmp != NULL && bufLen > 0 && GescomClients[index] != FREERTOS_INVALID_SOCKET)
			{
				FreeRTOS_send(GescomClients[index], msgBuffer, bufLen, /// FREERTOS_ZERO_COPY // 0);
			}
		}
	}
}
*/

void initLanGescomTasks(void)
{
	uint8_t i;
	for (i = 0; i < NUM_GES_COM_LAN; ++i)
	{
		GescomClients[i] = FREERTOS_INVALID_SOCKET;
		lgcQueueTx[i] = xQueueCreate(LAN_GESCOM_QUEUE_LENGTH, sizeof(SielMessage_t));
	}
	gcInitDone = pdTRUE;
}

void cbOnClientConnectDisconnect(Socket_t xSocket, BaseType_t ulConnected)
{

}

void lgCloseSocket(uint8_t idx)
{
	if(GescomClients[idx] != FREERTOS_INVALID_SOCKET)
	{
		FreeRTOS_closesocket(GescomClients[idx]);
	}
}

void lgChangeTaskStatus(uint8_t idx, uint8_t newStatus)
{
	printf("GESCOM %hu status change %hu -> %hu\n", idx, taskStatus[idx], newStatus);
	taskStatus[idx] = newStatus;
}

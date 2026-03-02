/*
 * lancom_bridge.c
 *
 *  Created on: Feb 24, 2026
 *      Author: Siel Tre
 */
#include "appl_tasks.h"
#include "spi_uart.h"
#include "data.h"
#include "FreeRTOS_IP.h"
#include "lancom.h"

#define ACCEPTOR_TASK_STACK_SIZE		1024
#define BRIDGE_TASK_STACK_SIZE			1536

SocketSet_t		xSocketSet;
static StaticEventGroup_t lan2ComEventGroup;
static uint8_t lbTaskStatus[NUM_PORTE_LAN_COM];

const osThreadAttr_t acceptorTask_attributes[NUM_PORTE_LAN_COM] = {
	{
	  .name = "LanCom0",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "LanCom1",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "LanCom2",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "LanCom3",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "LanCom4",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},{
	  .name = "LanCom5",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "LanCom6",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "LanCom7",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "LanCom8",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	},
	{
	  .name = "LanCom9",
	  .stack_size = ACCEPTOR_TASK_STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	}
};

const osThreadAttr_t combridgeTask_attributes = {
  .name = "ComBridge",
  .stack_size = BRIDGE_TASK_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal
};


void vTCPServerLancomAcceptTask(void *pvParameters)
{
	uint32_t index = (uint32_t)pvParameters;
	Socket_t xListeningSocket;
	BaseType_t xReuseSocket = pdTRUE;
	struct freertos_sockaddr xBindAddress;
	struct freertos_sockaddr xClientAddress;
	socklen_t xSize = sizeof(xClientAddress);
	for(;;)
	{
		if(lbTaskStatus[index] == TASK_SUSPENDED)
		{
			osDelay(pdMS_TO_TICKS(100));
			continue;
		}
		xListeningSocket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP);
		configASSERT(xListeningSocket != FREERTOS_INVALID_SOCKET);
		FreeRTOS_setsockopt(xListeningSocket, 0, FREERTOS_SO_REUSE_LISTEN_SOCKET, ( void * ) &xReuseSocket, sizeof( xReuseSocket ));
		xBindAddress.sin_port = FreeRTOS_htons(masterParametri.param_lan_com[index].porta_servizio);
		FreeRTOS_bind(xListeningSocket, &xBindAddress, sizeof(xBindAddress));
		FreeRTOS_listen(xListeningSocket, 1);
		// 4. Accept: il task si blocca finché un client non si connette
		printf("LANCOM %lu LISTEN\n", index);
		LancomClients[index] = FreeRTOS_accept(xListeningSocket, &xClientAddress, &xSize);

		if (LancomClients[index] != FREERTOS_INVALID_SOCKET) {

			printf("LANCOM %lu ACCEPT\n", index);
			//Aggiungiamo il socket al socket set
			FreeRTOS_FD_SET(LancomClients[index], xSocketSet, eSELECT_READ | eSELECT_EXCEPT);
			//TickType_t xTimeOut = pdMS_TO_TICKS(5000);
			//FreeRTOS_setsockopt(GescomClients[index], 0, FREERTOS_SO_RCVTIMEO, &xTimeOut, sizeof(xTimeOut));
			//Attendiamo finché il client non ritorna ad essere invalido
			while(LancomClients[index] != FREERTOS_INVALID_SOCKET)
			{
				if(lbTaskStatus[index] == TASK_SUSPENDED)
				{
                    FreeRTOS_closesocket(LancomClients[index]);
                    break;
				}
				osDelay(pdMS_TO_TICKS(100));
			}
		}
    }
}

void taskLanComBridge(void *pvParameters)
{
	const BaseType_t xClearBits = pdTRUE, xWaitAll = pdFALSE;
    uint8_t ucBuffer[DIMBUF_LAN_COM];
    EventBits_t uxBits;
    xSocketSet = FreeRTOS_CreateSocketSet();
    for(;;) {
        // Blocca il task finché succede qualcosa su ALMENO uno dei socket nel set
        // Il timeout (es. 50ms) permette di controllare anche le code COM periodicamente
        FreeRTOS_select(xSocketSet, pdMS_TO_TICKS(50));

        for(int i = 0; i < 10; i++) {
            if(LancomClients[i] == NULL) continue;
            int32_t lBytes;
        	CircularBuffer_t *pBufTx = (CircularBuffer_t *)&(bufferSpiTx[i]);
        	CircularBuffer_t *pBufRx = (CircularBuffer_t *)&(bufferSpiRx[i]);
            // Controlliamo se il socket 'i' ha dati in ingresso o eventi (es. chiusura)
            BaseType_t xEvent = FreeRTOS_FD_ISSET(LancomClients[i], xSocketSet);

            if (xEvent & eSELECT_EXCEPT)
			{
				FreeRTOS_FD_CLR(LancomClients[i], xSocketSet, eSELECT_ALL);
				FreeRTOS_closesocket(LancomClients[i]);
                printf("LANCOM %d socket GENERIC EXCEPTION\n", i);
				LancomClients[i] = FREERTOS_INVALID_SOCKET;
				continue;
			}

            if(xEvent & eSELECT_READ) {
                lBytes = FreeRTOS_recv(LancomClients[i], ucBuffer, sizeof(ucBuffer), 0);

                if(lBytes > 0) {
                    // Inoltra i dati alla COM corrispondente (es. tramite indice i)
                	cbPushLinear(pBufTx, ucBuffer, (uint16_t)lBytes);
                	xEventGroupSetBits(lan2ComEvGroupHandle, GET_BIT_EVENT(i));
                }
                else if(lBytes < 0 && lBytes != -pdFREERTOS_ERRNO_EAGAIN) {
                    // Il client ha chiuso o la connessione è caduta
                    FreeRTOS_FD_CLR(LancomClients[i], xSocketSet, eSELECT_ALL);
                    FreeRTOS_closesocket(LancomClients[i]);
                    LancomClients[i] = FREERTOS_INVALID_SOCKET;
                    printf("LANCOM %d socket ERROR: %ld\n", i, lBytes);
                    continue;
                }
            }

            // --- DIREZIONE COM -> LAN ---
            // Controlliamo se ci sono dati dalla COM 'i' (anche senza evento socket)
            uxBits = xEventGroupWaitBits(com2LanEvGroupHandle, GET_BIT_EVENT(i), xClearBits, xWaitAll, 0);
            if((uxBits & GET_BIT_EVENT(i)) == GET_BIT_EVENT(i))
            {
            	uint16_t uiAvail = cbPopLinear(pBufRx, ucBuffer, DIMBUF_LAN_COM);
                lBytes = FreeRTOS_send(LancomClients[i], ucBuffer, uiAvail, 0);
                // configASSERT( lBytes > 0 );
            }
        }
    }
}

void initLanComBridgeTask(void)
{
	lan2ComEvGroupHandle  = xEventGroupCreateStatic( &lan2ComEventGroup );
}

osThreadId_t createTask_TcpComServerAcceptor(void *arg)
{
	uint32_t index = (uint32_t)arg;
	return osThreadNew(vTCPServerLancomAcceptTask, arg, &acceptorTask_attributes[index]);
}

osThreadId_t createTask_LancomBridge(void *arg)
{
	initLanComBridgeTask();
	return osThreadNew(taskLanComBridge, arg, &combridgeTask_attributes);
}

void lbCloseSocket(uint8_t idx)
{
	if(LancomClients[idx] != FREERTOS_INVALID_SOCKET)
	{
		FreeRTOS_closesocket(LancomClients[idx]);
	}
}

void lbChangeTaskStatus(uint8_t idx, uint8_t newStatus)
{
	printf("LANCOM %hu status change %hu -> %hu\n", idx, lbTaskStatus[idx], newStatus);
	lbTaskStatus[idx] = newStatus;
}



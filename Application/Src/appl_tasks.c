/*
 * tasks.c
 *
 *  Created on: Feb 12, 2026
 *      Author: Siel Tre
 */
#include "appl_tasks.h"
#include "common_data.h"
#include "defs.h"
#include "lancom.h"

int lastThreadId = 0;
osThreadId_t tasksIdList[30];

ApplicationThread_t termSvrTasks[] = {
		{ createTask_GestIO, NULL },
		{ createTask_Spi2Com, NULL },
		{ createTask_LancomBridge, NULL },
		{ createTask_TcpComServerAcceptor, 0 },
		{ createTask_TcpComServerAcceptor, (void *)1 },
		{ createTask_TcpComServerAcceptor, (void *)2 },
		{ createTask_TcpComServerAcceptor, (void *)3 },
		{ createTask_TcpComServerAcceptor, (void *)4 },
		{ createTask_TcpComServerAcceptor, (void *)5 },
		{ createTask_TcpComServerAcceptor, (void *)6 },
		{ createTask_TcpComServerAcceptor, (void *)7 },
		{ createTask_TcpComServerAcceptor, (void *)8 },
		{ createTask_TcpComServerAcceptor, (void *)9 },
		{ NULL, NULL }
};

/**
 * @brief Routine di partenza di tutti i task dell'applicazione
 *
 * Immagazzina i dati all'interno della struttura tasksIdList
 */
void startApplicationTasks(void)
{
	int i = 0;
	pApplServersStop = atServersStop;
	pApplServersStart = atServersStart;
	while(termSvrTasks[i].func != NULL)
	{
		tasksIdList[lastThreadId++] = termSvrTasks[i].func(termSvrTasks[i].arg);
		i++;
	}
}

void atServersStop (void)
{
	for(int i = 0; i < NUM_PORTE_LAN_COM; ++i)
	{
		lbSuspendTask(i);
		osDelay(100);
	}
}

void atServersStart (void)
{
	for(int i = 0; i < NUM_PORTE_LAN_COM; ++i)
	{
		lbResumeTask(i);
		osDelay(100);
	}
}


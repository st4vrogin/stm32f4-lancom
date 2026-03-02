/*
 * tasks.c
 *
 *  Created on: Feb 12, 2026
 *      Author: Siel Tre
 */
#include "common_tasks.h"
#include <string.h>

int cmLastThreadId = 0;
osThreadId_t cmTasksIdList[30];

ApplicationThread_t commonTasks[] = {
		{ createTask_Ticker, NULL },
		{ createTask_VerificaConnessione, NULL },
		{ createTask_AutomaPing, NULL },
		{ createTask_Gescom, NULL },
		{ createTask_UsbCommRx, NULL },
		{ createTask_UsbCommTx, NULL },
		{ createTask_LanGescomRx, 0 },
		//{ createTask_LanGescomTx, 0 },
		{ createTask_LanGescomRx, (void *)1 },
		//{ createTask_LanGescomTx, (void *)1 },
		{ createTask_LanGescomRx, (void *)2 },
		//{ createTask_LanGescomTx, (void *)2 },
		{ NULL, NULL }
};

/**
 * @brief Routine di partenza di tutti i task comuni dell'applicazione
 *
 * Immagazzina i dati all'interno della struttura cmTasksIdList
 */
void startCommonTasks(void)
{
	int i = 0;
	while(commonTasks[i].func != NULL)
	{
		cmTasksIdList[cmLastThreadId++] = commonTasks[i].func(commonTasks[i].arg);
		i++;
	}
}

osThreadId_t	findThreadByName(const char* name, const osThreadId_t* appThreads)
{
	if(name == NULL || appThreads == NULL)
		return NULL;
	osThreadId_t* ptrAppThread = (osThreadId_t*)appThreads;
	const char *my_name = NULL;
	while(ptrAppThread != NULL && *ptrAppThread != NULL)
	{
		my_name = osThreadGetName(*ptrAppThread);
		if(strcmp(my_name, name) == 0)
			return *ptrAppThread;
		ptrAppThread++;
	}
	return NULL;
}

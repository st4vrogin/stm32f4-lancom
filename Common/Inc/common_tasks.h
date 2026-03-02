/*
 * common_tasks.h
 *
 *  Created on: Feb 13, 2026
 *      Author: Siel Tre
 */

#ifndef COMMON_INC_COMMON_TASKS_H_
#define COMMON_INC_COMMON_TASKS_H_
#include "cmsis_os2.h"

#define DEFAULT_STACK_SIZE			128 * 4

typedef osThreadId_t (*ApplicationThreadFunctor_t)(void* arg);
typedef struct
{
	ApplicationThreadFunctor_t func;
	void* arg;
} ApplicationThread_t;

osThreadId_t 	createTask_UsbCommRx(void* arg);
osThreadId_t 	createTask_UsbCommTx(void* arg);
osThreadId_t 	createTask_Gescom(void* arg);
osThreadId_t 	createTask_LanGescomRx(void* arg);
osThreadId_t 	createTask_LanGescomTx(void* arg);
osThreadId_t 	createTask_LanGescomDp(void* arg);
osThreadId_t 	createTask_Ticker(void* arg);
osThreadId_t 	createTask_AutomaPing(void* arg);
osThreadId_t 	createTask_VerificaConnessione(void* arg);

void 			startCommonTasks(void);

osThreadId_t	findThreadByName(const char* name, const osThreadId_t* appThreads);

#endif /* COMMON_INC_COMMON_TASKS_H_ */

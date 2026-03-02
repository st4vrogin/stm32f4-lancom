/*
 * tasks.h
 *
 *  Created on: Feb 11, 2026
 *      Author: SielTre
 */

#ifndef APPLICATION_INC_APPL_TASKS_H_
#define APPLICATION_INC_APPL_TASKS_H_
#include "common_tasks.h"

#define GEST_IO_DEFAULT_WAIT_MS		5


osThreadId_t createTask_GestIO(void* arg);
osThreadId_t createTask_Spi2Com(void* arg);
osThreadId_t createTask_TcpComServerAcceptor(void *arg);
osThreadId_t createTask_LancomBridge(void *arg);

void atServersStop (void);
void atServersStart (void);


void startApplicationTasks(void);


#endif /* APPLICATION_INC_APPL_TASKS_H_ */

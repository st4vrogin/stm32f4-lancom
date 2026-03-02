/*
 * lancom.h
 *
 *  Created on: Feb 25, 2026
 *      Author: Siel Tre
 */

#ifndef APPLICATION_INC_LANCOM_H_
#define APPLICATION_INC_LANCOM_H_
#include "FreeRTOS.h"
#include "circ_buffer.h"

#define GET_BIT_EVENT(x)		(1 << x)

/*
 * @brief Verifica se ci sono dati da inoltrare
 *
 * @param buf Il buffer circolare con i dati
 * @param com_index L'indice della porta COM
 * @return pdTRUE se si, pdFALSE altrimenti
 */
BaseType_t lchHasDataToForward(const CircularBuffer_t* buf, uint8_t com_index);

/**
 * @brief Chiude la connessione al bridge lancom
 *
 * @param idx L'indice della porta su cui chiudere la connessione
 */
void lbCloseSocket(uint8_t idx);

void lbChangeTaskStatus(uint8_t idx, uint8_t newStatus);

#define 	lbSuspendTask(x)		lbChangeTaskStatus((x), TASK_SUSPENDED)
#define 	lbResumeTask(x)			lbChangeTaskStatus((x), TASK_RUNNING)


#endif /* APPLICATION_INC_LANCOM_H_ */

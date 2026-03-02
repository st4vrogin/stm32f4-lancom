/*
 * lan_init.h
 *
 *  Created on: Feb 20, 2026
 *      Author: Siel Tre
 */

#ifndef COMMON_INC_LAN_INIT_H_
#define COMMON_INC_LAN_INIT_H_

/**
 * @brief Reinizializza i parametri dell'interfaccia Lan da RAM
 *
 * Usata solo all'avvio
 */
void liInitParametersRam(void);

/**
 * @brief Inizializza l'interfaccia di rete e lo stack FreeRTOS
 *
 * Usata solo all'avvio
 */
void liFreeRtosIpInit(void);
void liFreeRtosIpReinit(void);

/**
 * @brief Ferma i server in ascolto sulle porte
 *
 ** E' necessario definire la corrispondente funzione in ambito applicativo
 */
void liStopServers(void);


/**
 * @brief Accende i server in ascolto sulle porte
 *
 ** E' necessario definire la corrispondente funzione in ambito applicativo
 */
void liStartServers(void);

//Funzione extern per la chiusura dei socket gescom
void lgCloseSocket(uint8_t idx);

void lgChangeTaskStatus(uint8_t idx, uint8_t newStatus);


#define 	lgSuspendTask(x)		lgChangeTaskStatus((x), TASK_SUSPENDED)
#define 	lgResumeTask(x)			lgChangeTaskStatus((x), TASK_RUNNING)

#endif /* COMMON_INC_LAN_INIT_H_ */

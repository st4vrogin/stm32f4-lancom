/*
 * mem_siel_rtos.h
 *
 *  Created on: Feb 13, 2026
 *      Author: Siel Tre
 */
#ifndef COMMON_PROTOCOL_MEM_SIEL_RTOS_H_
#define COMMON_PROTOCOL_MEM_SIEL_RTOS_H_
#include <stddef.h>
#include "stm32f4xx_hal.h"


#define BLANK_FLASH				0xFFFFFFFF		// valore (32bit) della flash appena cancellata

#define	MEM_ID_UNKNOWN		-1
#define	MEM_ID_SRAM			0
#define	MEM_ID_SDRAM		1
#define	MEM_ID_REG			2
#define	MEM_ID_FLASH		3
#define	MEM_ID_FLASH_EXT	4

#define	SRAM_196K			0x30000

#define	FLASH_EXT_SPI_BASE	0xB0000000		// La scelta dell'indirizzo è ricaduta in questo range in quanto è una sezione
											// con accesso riservato dal cortex F4
#define	LEN_FLASH_EXT_SPI	0x00080000		// 8 MB di flash ext spi

#define ERASE_FLASH_OK		0xFFFFFFFF

/**
 * @brief Routine per allocare memoria da Heap
 *
 * Clone di malloc(), ma utilizza la libreria FreeRTOS
 *
 * @param size La dimensione di memoria da allocare
 * @return Puntatore all'area di memoria allocata o NULL se problemi
 */
void *msrMalloc(size_t size);

/**
 * @brief Routine per allocare memoria da Heap e inizializzare l'area di memoria
 *
 * Clone di calloc(), ma utilizza la libreria FreeRTOS
 *
 * @param size La dimensione di memoria da allocare
 * @return Puntatore all'area di memoria allocata o NULL se problemi
 */
void *msrCalloc(size_t size);

/**
 * @brief Routine per riallocare la memoria da un'area di memoria precedentemente allocata
 *
 * Clone di realloc(), ma utilizza la libreria FreeRTOS
 *
 * @param orig L'area originale di memoria già allocata da malloc e simili
 * @param size La dimensione di memoria da allocare
 * @return Puntatore all'area di memoria allocata o NULL se problemi
 */
void *msrRealloc(void * orig, size_t size);

/**
 * @brief Routine per liberare la memoria allocata da Heap
 *
 * Clone di free(), ma utilizza la libreria FreeRTOS
 *
 * @param ptr L'area originale di memoria già allocata da malloc e simili
 */
void msrFree(void *ptr);

/**
 * @brief Resituisce il tipo di memoria a partire dall'indirizzo assoluto
 *
 * @param Indirizzo di memoria
 * @return Tipo di memoria individuato
 */
int8_t  msrGetMemType(uint32_t adr);

/**
 * @brief Scrive la flash a blocchi di 4 byte
 *
 * @param Indirizzo di destinazione
 * @param Indirizzo di origine
 * @param Lunghezza del buffer di origine
 */
HAL_StatusTypeDef msrFlashWrite_u32(uint32_t dest, uint32_t *punt_src, uint32_t len_src);

/**
 * @brief Scrive la flash byte per byte
 *
 * @param Indirizzo di destinazione
 * @param Indirizzo di origine
 * @param Lunghezza del buffer di origine
 */
HAL_StatusTypeDef msrFlashWrite_u8(uint32_t dest, uint8_t *punt_src, uint32_t len_src);

/**
 * @brief Cancella la flash calcolando i settori a partire dalla lunghezza dichiarata
 *
 * Questa routine cancella la flash a settori, per cui bisogna prestare molta attenzione
 * al settore in cui cade quell'indirizzo, per evitare che vengano cancellati dati che non
 * c'entrano e che magari sono necessari.
 *
 * @param Indirizzo di cancellazione
 * @param Lunghezza minima necessaria
 */
uint32_t msrFlashErase(uint32_t dest, uint32_t len_src);

#endif

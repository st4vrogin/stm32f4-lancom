/*
 * circ_buffer.h
 *
 *  Created on: Feb 25, 2026
 *      Author: Siel Tre
 */

#ifndef APPLICATION_INC_CIRC_BUFFER_H_
#define APPLICATION_INC_CIRC_BUFFER_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"

#define BUFFER_SIZE 256 // Deve essere una potenza di 2 per ottimizzazioni (opzionale)

typedef struct {
    uint8_t data[BUFFER_SIZE];
    uint16_t head;   // Indice di scrittura
    uint16_t tail;   // Indice di lettura
    uint16_t count;  // Numero di elementi attuali
} StaticCircularBuffer_t;

typedef struct {
    uint8_t *data;
    uint16_t size;	 // Dimensione allocata
    uint16_t head;   // Indice di scrittura
    uint16_t tail;   // Indice di lettura
    uint16_t count;  // Numero di elementi attuali
} CircularBuffer_t;

/**
 * @brief Esegue l'inizializzazione di un buffer circolare statico
 */
void cbInitializeStatic(StaticCircularBuffer_t *cb);


/**
 * @brief Esegue l'inizializzazione di un buffer circolare dinamico
 *
 * @param cb Puntatore alla struttura buffer
 * @param size Dimensione richiesta
 * @return pdTRUE se l'operazione ha avuto successo, pdFALSE altrimenti
 */
BaseType_t cbInitialize(CircularBuffer_t *cb, uint16_t size);

/**
 * @brief Distrugge un buffer circolare dinamico
 *
 * @param cb Puntatore alla struttura buffer
 */
void cbDestroy(CircularBuffer_t *cb);


/**
 * @brief Riempie il buffer circolare a partire da un buffer lineare
 *
 * @param cb Puntatore alla struttura del buffer circolare
 * @param src Puntatore al buffer sorgente
 * @param length Lunghezza del buffer sorgente
 */
void cbPushLinear(CircularBuffer_t *cb, const uint8_t *src, int length);
void cbPushLinearStatic(StaticCircularBuffer_t *cb, const uint8_t *src, int length);

/**
 * @brief Riempie il buffer lineare di destinazione a partire da un buffer circolare
 *
 * @param cb Puntatore alla struttura del buffer circolare
 * @param dest Puntatore al buffer di destinazione
 * @param length Lunghezza massima del buffer di destinazione
 */
uint16_t cbPopLinear(CircularBuffer_t *cb, uint8_t *dest, uint16_t length);

#endif /* APPLICATION_INC_CIRC_BUFFER_H_ */

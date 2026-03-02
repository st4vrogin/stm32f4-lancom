/*
 * circ_buffer.c
 *
 *  Created on: Feb 25, 2026
 *      Author: Siel Tre
 */
#include "circ_buffer.h"
#include <string.h>
#include "mem_siel_rtos.h"

void cbInitializeStatic(StaticCircularBuffer_t *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    memset(cb->data, 0, BUFFER_SIZE);
}

BaseType_t cbInitialize(CircularBuffer_t *cb, uint16_t size) {
	cb->data = NULL;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->data = msrMalloc(size);
    if(cb->data != NULL) {
    	cb->size = size;
    	memset(cb->data, 0, cb->size);
    	return pdTRUE;
    }
    return pdFALSE;
}

void cbDestroy(CircularBuffer_t *cb)
{
	if(cb != NULL && cb->data != NULL)
	{
		msrFree(cb->data);
		cb->size = 0;
	}
}

void cbPushLinear(CircularBuffer_t *cb, const uint8_t *src, int length)
{
	if (cb == NULL || cb->data == NULL || cb->size == 0) return;
	// Se i dati in entrata sono più grandi del buffer, teniamo solo gli ultimi
	if (length > cb->size) {
		src += (length - cb->size);
		length = cb->size;
	}

	// Calcoliamo lo spazio fino alla fine fisica dell'array
	int space_to_end = cb->size - cb->head;

	if (length <= space_to_end) {
		// Caso 1: La copia è lineare e non deve ricominciare da capo
		memcpy(&(cb->data[cb->head]), src, length);
	} else {
		// Caso 2: Dobbiamo spezzare la copia in due parti
		memcpy(&(cb->data[cb->head]), src, space_to_end);
		memcpy(&(cb->data[0]), src + space_to_end, length - space_to_end);
	}

	// Aggiornamento Indici
	cb->head = (cb->head + length) % cb->size;

	// Gestione sovrascrittura: se i nuovi dati superano la capacità attuale,
	// la coda (tail) viene spinta avanti per mantenere la coerenza.
	if (cb->count + length > cb->size) {
		cb->count = cb->size;
		cb->tail = cb->head; // In overwrite totale, la coda insegue la testa
	} else {
		cb->count += length;
	}
}

uint16_t cbPopLinear(CircularBuffer_t *cb, uint8_t *dest, uint16_t length)
{
	uint16_t bytes_to_read;
	if (cb == NULL || cb->data == NULL || cb->size == 0) return 0;
    // Non possiamo leggere più di quanto sia effettivamente presente
	bytes_to_read = (length < cb->count) ? length : cb->count;
    if (bytes_to_read == 0 || bytes_to_read > cb->size) return 0;
    uint16_t space_to_end = cb->size - cb->tail;
    if (bytes_to_read <= space_to_end) {
        // Lettura lineare
        memcpy(dest, &(cb->data[cb->tail]), bytes_to_read);
    } else {
        // Lettura con wrap-around
        memcpy(dest, &(cb->data[cb->tail]), space_to_end);
        memcpy(dest + space_to_end, &(cb->data[0]), bytes_to_read - space_to_end);
    }
    // Aggiornamento indici
    cb->tail = (cb->tail + bytes_to_read) % BUFFER_SIZE;
    cb->count -= bytes_to_read;
    return bytes_to_read;
}

/*
 * Protocol/SielPtcl.h
 *
 *  Created on: Feb 12, 2026
 *      Author: Siel Tre
 */
#ifndef COMMON_INC_PROTOCOL_SIELPTCL_H_
#define COMMON_INC_PROTOCOL_SIELPTCL_H_
#include "SielType.h"

/**
 * @brief Esegue lo stuff del messaggio \ref SielMessage_t
 *
 * In caso di ritorno NULL è posssibile chiamare la routine spErrno
 * per maggiori informazioni sull'errore accaduto
 *
 * @param msg Il messaggio da stuffare
 * @param dest Il buffer di destinazione
 * @param len_buf La lunghezza del buffer di destinazione
 * @return Il buffer di destinazione se l'operazione è andata a buon fine, NULL altrimenti
 */
uint8_t*        spStuffMessage(const SielMessage_t* msg, uint8_t* comDati, uint16_t* len_buf);

/**
 * @brief Esegue il destuff di un buffer su messaggio \ref SielMessage_t
 *
 * In caso di ritorno NULL è posssibile chiamare la routine spErrno
 * per maggiori informazioni sull'errore accaduto
 *
 * @param src Il buffer di origine
 * @param len_buf La lunghezza del buffer di origine
 * @param msg Il messaggio di destinazione
 * @return Il messaggio di destinazione se l'operazione è andata a buon fine, NULL altrimenti
 */
SielMessage_t*  spDestuffMessage(const uint8_t* dati, uint16_t len_dati, SielMessage_t* msg);

/**
 * @brief Esegue lo stuff del messaggio \ref SielMessage_t usando l'heap
 *
 * In caso di ritorno NULL è posssibile chiamare la routine spErrno
 * per maggiori informazioni sull'errore accaduto
 *
 * @param msg Il messaggio da stuffare
 * @param dest Il buffer di destinazione
 * @param len_buf La lunghezza del buffer di destinazione
 * @return Il buffer di destinazione se l'operazione è andata a buon fine, NULL altrimenti
 */
uint8_t*        spStuffMessageHeap(const SielMessage_t* msg, uint8_t** dest, uint16_t* len_buf);

/**
 * @brief Esegue il destuff di un buffer su messaggio \ref SielMessage_t usando l'heap
 *
 * In caso di ritorno NULL è posssibile chiamare la routine spErrno
 * per maggiori informazioni sull'errore accaduto
 *
 * @param src Il buffer di origine
 * @param len_buf La lunghezza del buffer di origine
 * @param msg Il messaggio di destinazione
 * @return Il messaggio di destinazione se l'operazione è andata a buon fine, NULL altrimenti
 */
SielMessage_t*  spDestuffMessageHeap(const uint8_t* dati, uint16_t len_dati, SielMessage_t** msg);

/**
 * @brief Calcolo della checksum su 8 bit
 *
 * @param punt_buffer buffer su cui calcolare la checksum
 * @param lunghezza La lunghezza utile per il calcolo
 * @return Checksum su 8 bit
 */
uint8_t	        calcolo_chk8(uint8_t *punt_buffer, uint16_t lunghezza);

/**
 * @brief Ritorna l'ultimo errore rilevato dal modulo
 *
 * @return Ultimo errore rilevato dal modulo
 */
int             spErrno(void);

/**
 * @brief Calcola la checksum su un Messaggio \ref SielMessage_t
 *
 * @return Ultimo errore rilevato dal modulo
 */
uint8_t         CalcolaChecksum(const SielMessage_t* msg);

/**
 * @brief Conta il numero di byte da stuffare o destuffare a seconda dell'operazione
 *
 * @param buf Buffer su cui eseguire l'operazione
 * @param len Lunghezza utile del buffer
 * @param op Operazione COUNT_STUFF o COUNT_DESTUFF
 * @return Numero di byte da stuffare/destuffare
 */
uint16_t		spCountInBuffer(const uint8_t* buf, uint16_t len, opCount_e op);

/**
 * @brief Routine a stati finiti che esegue il destuffing byte per byte su un buffer
 *
 * Ideale da inserire in un ciclo for che scorre tutti i byte di un messaggio
 *
 * @param rxByte Byte ricevuto
 * @param currStatus Stato corrente della macchina a stati finiti
 * @param dest Buffer di destinazione
 * @param idx Indirizzo della variabile contenente l'indice corrente
 * @return Nuovo stato della macchina a stati finiti
 */
e_InPlaceSielProtocolStatus  spDestuffBufferInPlace(const uint8_t rxByte, const e_InPlaceSielProtocolStatus currStatus, uint8_t *dest, uint16_t *idx);

#endif

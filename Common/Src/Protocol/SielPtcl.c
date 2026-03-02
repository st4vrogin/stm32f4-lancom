#include "Protocol/SielPtcl.h"
#include <stddef.h>
#include "FreeRTOS.h"
#include "mem_siel_rtos.h"

static int last_error;


uint8_t* spStuffMessage(const SielMessage_t* msg, uint8_t* dati, uint16_t* len_buf)
{
    int i = 0, j = 0;
    last_error = ERR_OK;
    uint8_t * pBuf;
    if(*len_buf < MAX_COM_OVHD + msg->header.len) {
        // Buffer troppo piccolo
        last_error = ERR_BUF_TOO_SMALL;
        return NULL;
    }
    pBuf = (uint8_t *)&(msg->gestore);
    dati[j++] = PTCL_BYTE_START; // STX
    for (i = 0; i < MAX_COM_OVHD; i++) {
        if (j >= *len_buf) 
        {
            // Buffer pieno, non possiamo continuare
            last_error = ERR_BUF_TOO_SMALL;
            return NULL;
        }
        switch(pBuf[i]) {
            case PTCL_BYTE_START:
            case PTCL_BYTE_STOP:
            case PTCL_BYTE_STUFF:
                dati[j++] = PTCL_BYTE_STUFF; // Byte Stuffing
                dati[j++] = pBuf[i] + PTCL_BYTE_DESTUFF; // Destuffing
                break;
            default:
                dati[j++] = pBuf[i];
                break;
        }
    }
    pBuf = msg->netto;
    for (i = 0; i < msg->header.len; i++)
    {
		if (j >= *len_buf)
		{
			// Buffer pieno, non possiamo continuare
			last_error = ERR_BUF_TOO_SMALL;
			return NULL;
		}
		switch(pBuf[i]) {
			case PTCL_BYTE_START:
			case PTCL_BYTE_STOP:
			case PTCL_BYTE_STUFF:
				dati[j++] = PTCL_BYTE_STUFF; // Byte Stuffing
				dati[j++] = pBuf[i] + PTCL_BYTE_DESTUFF; // Destuffing
				break;
			default:
				dati[j++] = pBuf[i];
				break;
		}
    }
    dati[j++] = PTCL_BYTE_STOP; // ETX
    *len_buf = j;
    return dati;
}


SielMessage_t*  spDestuffMessage(const uint8_t* src, uint16_t len_dati, SielMessage_t* msg)
{
    enum { WAIT_START, IN_MSG, STUFF_NEXT, DONE } state = WAIT_START;
    int i = 0, j = 0;
    last_error = ERR_OK;
    uint8_t* dst = (uint8_t*)msg;

    for (i = 0; i < len_dati && state != DONE; ++i) {
        switch (state) {
            case WAIT_START:
                if (src[i] == PTCL_BYTE_START) {
                    state = IN_MSG;
                }
                break;
            case IN_MSG:
                if (src[i] == PTCL_BYTE_STOP) {
                    state = DONE;
                } else if (src[i] == PTCL_BYTE_STUFF) {
                    state = STUFF_NEXT;
                } else {
                    dst[j++] = src[i];
                }
                break;
            case STUFF_NEXT:
                // Stuffed byte: destuff
                dst[j++] = src[i] - PTCL_BYTE_DESTUFF;
                state = IN_MSG;
                break;
            default:
                break;
        }
    }
    if (state != DONE) {
        last_error = ERR_INVALID_FORMAT;
        return NULL;
    }
    // msg->len should be set by caller or extracted from dst if needed
    msg->header.len = j - MAX_COM_OVHD;
    return msg;
}


uint8_t	calcolo_chk8(uint8_t *punt_buffer, uint16_t lunghezza)
{

	uint16_t 	i;
	uint8_t 	sum = 0;
	for (i = 0 ; i < lunghezza ; i++)
	{
		sum += punt_buffer[i];
	}

	return(0xFF - sum);
}

int spErrno(void)
{
    return last_error;
}


uint8_t         CalcolaChecksum(const SielMessage_t* msg)
{
	uint16_t i;
	uint8_t cksum = 0;
	uint8_t * punt_buf = (uint8_t*)&(msg->mitt[0]);
	for (i = 0; i < MAX_COM_OVHD - 1; i++)
	{
		cksum += punt_buf[i];
	}
	cksum = calcolo_chk8(msg->netto, msg->header.len) - cksum;
    return cksum;
}

uint16_t		spCountInBuffer(const uint8_t* buf, uint16_t len, opCount_e op)
{
	uint16_t i, cntr;
	cntr = 0;
	for (i = 0; i < len; i++)
	{
		if(op == COUNT_DESTUFF && buf[i] == PTCL_BYTE_STUFF)
			cntr++;
		else if (op == COUNT_STUFF)
		{
			switch(buf[i])
			{
				case PTCL_BYTE_START:
				case PTCL_BYTE_STOP:
				case PTCL_BYTE_STUFF:
					cntr++;
					break;
				default:
					break;
			}
		}
	}
	return cntr;
}

uint8_t*        spStuffMessageHeap(const SielMessage_t* msg, uint8_t** dest, uint16_t* len_buf)
{
	uint8_t *punt_msg;
	if(msg == NULL || msg->header.len == 0)
	{
		last_error = ERR_INVALID_FORMAT;
		return NULL;
	}
	if(dest == NULL)
	{
		last_error = ERR_INVALID_BUFFER;
		return NULL;
	}
	//Inizialmente dobbiamo contare quanti byte sono da stuffare nel messaggio
	punt_msg = (uint8_t *)(&msg + sizeof(msg->header));
	*len_buf = spCountInBuffer(punt_msg, MAX_COM_OVHD - 1, COUNT_STUFF);
	*len_buf += spCountInBuffer(msg->netto, msg->header.len, COUNT_STUFF);
	*len_buf += 2 + msg->header.len;
	*dest = (uint8_t *)msrMalloc(*len_buf);
	if(*dest == NULL)
	{
		*len_buf = 0;
		last_error = ERR_MEM;
		return NULL;
	}
	return spStuffMessage(msg, *dest, len_buf);
}

SielMessage_t*  spDestuffMessageHeap(const uint8_t* dati, uint16_t len_dati, SielMessage_t** msg)
{
	uint16_t sz;
	if(dati == NULL || len_dati == 0)
	{
		last_error = ERR_INVALID_BUFFER;
		return NULL;
	}
	if(msg == NULL)
	{
		last_error = ERR_INVALID_FORMAT;
		return NULL;
	}
	sz = spCountInBuffer(dati, len_dati, COUNT_DESTUFF);
	if (sz < 3)
	{
		last_error = ERR_INVALID_FORMAT;
		return NULL;
	}
	sz -= 2;
	*msg = (SielMessage_t *)msrCalloc(sizeof(SielMessage_t));
	if (*msg == NULL) { last_error = ERR_MEM; return NULL; }
	(*msg)->netto = (uint8_t *)msrMalloc(sz);
	if((*msg)->netto == NULL) { last_error = ERR_MEM; return NULL; }
	return spDestuffMessage(dati, len_dati, *msg);
}

e_InPlaceSielProtocolStatus  spDestuffBufferInPlace(const uint8_t rxByte, const e_InPlaceSielProtocolStatus currStatus, uint8_t *dest, uint16_t *idx)
{
	e_InPlaceSielProtocolStatus sps_status = currStatus;
	switch(sps_status)
	{
		case SPS_WAIT_STX:
			if (rxByte == PTCL_BYTE_START)
			{
				sps_status = SPS_WAIT_ETX;
				*idx = 0;
			}
			break;
		case SPS_WAIT_ETX:
			if (rxByte == PTCL_BYTE_STOP)
				sps_status = SPS_END;
			else if (rxByte == PTCL_BYTE_STUFF)
				sps_status = SPS_SPECIAL_BYTE;
			else
				dest[(*idx)++] = rxByte;
			break;
		case SPS_SPECIAL_BYTE:
			dest[(*idx)++] = (uint8_t)(rxByte - PTCL_BYTE_DESTUFF);
			sps_status = SPS_WAIT_ETX;
			break;
		default:
			sps_status = SPS_WAIT_STX;
			*idx = 0;
			break;

	}
	return sps_status;
}

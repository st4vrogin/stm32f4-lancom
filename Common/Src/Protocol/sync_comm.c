/*
 * sync_comm.c
 *
 *  Created on: Feb 13, 2026
 *      Author: m.nicoloso
 */
#include "Protocol/sync_comm.h"
#include "mem_siel_rtos.h"
#include "Protocol/SielPtcl.h"
#include "Protocol/SielCodop.h"
#include <string.h>

int GestisciComunicazioneSincronaSlave(const SerialPacket_t *pkt, uint8_t **tmp_buf, uint16_t *len_buf)
{
	uint16_t i;
	uint8_t *bufPtr;
	SielMessage_t *message, *resp;
	if (pkt == NULL)
		return ERR_COMM_GENERIC;
	if (*tmp_buf == NULL)
	{
		*tmp_buf = (uint8_t *)msrMalloc(pkt->len);
		if(*tmp_buf != NULL)
		{
			memcpy(*tmp_buf, pkt->data, pkt->len);
			*len_buf = pkt->len;
		}
		else
		{
			*len_buf = 0;
			return ERR_COMM_MEM;
		}
	}
	else
	{
		bufPtr = (uint8_t *)msrRealloc(*tmp_buf, *len_buf + pkt->len);
		if(bufPtr != NULL)
		{
			*tmp_buf = bufPtr;
			memcpy(&(*tmp_buf[*len_buf]), pkt->data, pkt->len);
			*len_buf += pkt->len;
		}
		else
		{
			*len_buf = 0;
			return ERR_COMM_MEM;
		}
	}
	i = *len_buf - 1;
	while (i > 0)
	{
		if(*tmp_buf[i] == PTCL_BYTE_STOP)
			break;
		i--;
	}
	if (i == 0)
		return ERR_COMM_INCOMPLETE;
	message = spDestuffMessageHeap(*tmp_buf, i+1, &message);
	msrFree(*tmp_buf);	*len_buf = 0;

	if(message == NULL)
	{
		return ERR_COMM_GENERIC;
	}
	resp = GestisciCodop(message);
	if(resp != NULL && resp->len > 0)
	{
		*tmp_buf = spStuffMessageHeap(resp, tmp_buf, len_buf);
		if(*tmp_buf == NULL) return ERR_COMM_MEM;
		return ERR_OK;
	}
	return ERR_NO_ANS;
}

/*
 * lancom_helper.c
 *
 *  Created on: Feb 25, 2026
 *      Author: Siel Tre
 */
#include "data.h"
#include "lancom.h"

#define BUF_MAX_PERC		90

BaseType_t lchHasDataToForward(const CircularBuffer_t* buf, uint8_t com_index)
{
	uint16_t cur_ptr;
	if(buf == NULL || com_index >= NUM_PORTE_COM || buf->size == 0)
		return pdFALSE;

	if(buf->count == 0)
		return pdFALSE;

	if (masterParametri.param_lan_com[com_index].len_max > 0 && buf->count >= masterParametri.param_lan_com[com_index].len_max)
		return pdTRUE;

	if (masterParametri.param_lan_com[com_index].timeout_send_tx > 0 && MasterTimer_u16_5mS.LanCom[com_index] >= masterParametri.param_lan_com[com_index].timeout_send_tx)
		return pdTRUE;

	cur_ptr = buf->head;
	if (masterParametri.param_lan_com[com_index].usa_carattere_per_tx == 0x01)
	{
		while (cur_ptr != buf->tail)
		{
			if (buf->data[cur_ptr] == masterParametri.param_lan_com[com_index].carattere_per_tx)
				return pdTRUE;
			cur_ptr++;
			if(cur_ptr == buf->size)
				cur_ptr = 0;
		}
	}

	//Se siamo arrivati comunque al riempimento massimo, anche senza aver superato i check, invio lo stesso
	if(buf->count > (DIMBUF_LAN_COM * 100) / 90)
		return pdTRUE;

	return pdFALSE;
}

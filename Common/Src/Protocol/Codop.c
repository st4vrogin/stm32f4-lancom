/*
 * Codop.c
 *
 *  Created on: Feb 14, 2026
 *      Author: Siel Tre
 */
#include "Protocol/Codop.h"
#include "common_data.h"
#include "mem_siel_rtos.h"
#include "sbanc.h"
#include "LoaderStub.h"

/*****************************************************************
	Variabili extern
******************************************************************/
#ifdef USA_LOADER
extern char start_of_LDR_SW_DESC[];
#else
char start_of_LDR_SW_DESC[] = {
		2, 24, 2, 24, 0, 0, 0, 26, 2, 26, 0, 0, 26, 2, 26, 9, 12, 32, 0, 0, 0, 0, 0, 0
};
#endif
//extern uint8_t param_sbanc_ram[];

/*****************************************************************
	Costanti locali
******************************************************************/
const info_ver_ldr_t *ptrLoaderVersion = (const info_ver_ldr_t *)start_of_LDR_SW_DESC;

/*****************************************************************
	cod_op_02:
		parametri:
			uint8	subcode			0 -> dati del processore
									1 -> versione kernel
		dati restituiti:
			vedi strutture dati
******************************************************************/
uint16_t  	cod_op_02(void)
{
	uint16_t rispLen = 0;
	p_cop_02_00_t		*parm_00_tx = (p_cop_02_00_t *)&(ptrBufCom->buf.cmd.dati[1]);
	switch (ptrBufCom->buf.cmd.dati[0])
	{
		case 0:
			// Dati del processore
			parm_00_tx->device_id = 	(DBGMCU->IDCODE & 0xFFF);
			parm_00_tx->uid[0] = 		UID->uid[0];
			parm_00_tx->uid[1] = 		UID->uid[1];
			parm_00_tx->uid[2] = 		UID->uid[2];
			parm_00_tx->flash_size =	FLASH_SIZE->flash_size;
			rispLen += sizeof(parm_00_tx->device_id) + sizeof(parm_00_tx->uid) + sizeof(parm_00_tx->flash_size) + 1;	// +1 perch� aggiungo anche il sub_code
			break;
		case 1:
			// TODO: Versione FW del loader
			memcpy_secure((void *)&(ptrBufCom->buf.cmd.dati[1]), sizeof(ptrBufCom->buf.cmd.dati) -1, (void *)&(ptrLoaderVersion->Version), sizeof(version_app_t));
			rispLen += /* sizeof(version_app_t)  */ 8 + 1; // +1 perché aggiungo anche il sub_code
			break;
		default:
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			break;
	}
	return(rispLen);
}

/*****************************************************************
	cod_op_04: lettura memoria su 4 byte adr e len dati
		parametri:
			uint32_t	addr
			uint16_t	len
		dati restituiti:
			uint32_t	addr
			uint16_t	len
			uint8	buf[len]
******************************************************************/
uint16_t  	cod_op_04(void)
{
//	p_cop_04_06_0C_0E_t		*parm_rxtx = (p_cop_04_06_0C_0E_t *)&(ptrBufCom->buf.cmd.dati);
	p_cop_04_06_0C_0E_t		parm_rxtx;
	int8_t					tipo_memoria_start, tipo_memoria_end;
	uint16_t rispLen = 0;

	parm_rxtx.addr = ptrBufCom->buf.cmd.dati[0] + (ptrBufCom->buf.cmd.dati[1] << 8) + (ptrBufCom->buf.cmd.dati[2] << 16) + (ptrBufCom->buf.cmd.dati[3] << 24);
	parm_rxtx.len = ptrBufCom->buf.cmd.dati[4] + (ptrBufCom->buf.cmd.dati[5] << 8);

	rispLen += sizeof(parm_rxtx.addr) + sizeof(parm_rxtx.len);

	tipo_memoria_start = msrGetMemType(parm_rxtx.addr);
	tipo_memoria_end = msrGetMemType(parm_rxtx.addr + parm_rxtx.len - 1);

	if ((tipo_memoria_start == MEM_ID_UNKNOWN)	||
		(tipo_memoria_end == MEM_ID_UNKNOWN)	||
		(tipo_memoria_start != tipo_memoria_end))
	{
		// Zona di memoria con accesso non consentito
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
		return(rispLen);
	}

	if (parm_rxtx.len > MAX_COM_DATA_UART)		//dimensione uguale per tutti i Ges_com
	{
		// Buffer pi� lungo di quanto consentito
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
		return(rispLen);
	}

	switch(tipo_memoria_start)
	{
		case MEM_ID_SRAM:
		case MEM_ID_FLASH:
		case MEM_ID_REG:
			if (parm_rxtx.len < DIMBUF_GES_COM)
			{
				memcpy((uint8_t *)&(ptrBufCom->buf.cmd.dati[6]), (uint8_t *)parm_rxtx.addr, parm_rxtx.len);
			}
			else
			{
				ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
				return(rispLen);
			}

			break;
		default:
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			return(rispLen);
	}
	rispLen += parm_rxtx.len;
	return(rispLen);

}

/*****************************************************************
	cod_op_06: Scrittura memoria su 4 byte adr e len dati
		parametri:
			uint32_t	addr
			uint16_t	len
			uint8	buf[len]
		dati restituiti:
			uint32_t	addr
			uint16_t	len
******************************************************************/
uint16_t  	cod_op_06(void)
{
	p_cop_04_06_0C_0E_t		parm_rxtx;
	int8_t					tipo_memoria_start, tipo_memoria_end;
	uint16_t rispLen = 0;

	parm_rxtx.addr = ptrBufCom->buf.cmd.dati[0] + (ptrBufCom->buf.cmd.dati[1] << 8) + (ptrBufCom->buf.cmd.dati[2] << 16) + (ptrBufCom->buf.cmd.dati[3] << 24);
	parm_rxtx.len = ptrBufCom->buf.cmd.dati[4] + (ptrBufCom->buf.cmd.dati[5] << 8);

	rispLen += sizeof(parm_rxtx.addr) + sizeof(parm_rxtx.len);

	tipo_memoria_start = msrGetMemType(parm_rxtx.addr);
	tipo_memoria_end = msrGetMemType(parm_rxtx.addr + parm_rxtx.len - 1);


	if ((tipo_memoria_start == MEM_ID_UNKNOWN)	||
		(tipo_memoria_end == MEM_ID_UNKNOWN)	||
		(tipo_memoria_start != tipo_memoria_end))
	{
		// Zona di memoria con accesso non consentito
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
		return(rispLen);
	}

	switch (tipo_memoria_start)
	{
		case MEM_ID_SRAM:
		case MEM_ID_REG:
			// Scrittura in RAM
			memcpy((uint8_t *)parm_rxtx.addr, (uint8_t *)&(ptrBufCom->buf.cmd.dati[6]), parm_rxtx.len);
			break;
		case MEM_ID_FLASH:
			if (over_mem.sys_flag.bit.DisableFlash == 1)	//verifica permesso di accesso
			{
				ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_FLASH_DISABLE;
				break;
			}
			if (msrFlashWrite_u8(parm_rxtx.addr, (uint8_t *)&(ptrBufCom->buf.cmd.dati[6]), parm_rxtx.len) != HAL_OK)
			{
				ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			}
			break;
// Da spostare in AppCodOp.c ???
//		case MEM_ID_FLASH_EXT:
//			//FlashWriteData((parm_rxtx.addr & 0x0FFFFFFF), (uint8 *)&(ptrBufCom->buf.cmd.dati[6]), parm_rxtx.len);
//			ric_write_canc_fl_ext_spi (1, (parm_rxtx.addr & 0x0FFFFFFF),(uint8 *)&(ptrBufCom->buf.cmd.dati[6]), parm_rxtx.len);
//			break;
	}
	return(rispLen);
}

/****************************************************************
	p_cop_08_t: comandi parametrici
		parametri
			uint8	param
			uint8	dati[3]
		dati restituiti:
			null
****************************************************************/
uint16_t  	cod_op_08(void)
{
	p_cop_08_t		*parm_rx = (p_cop_08_t *)&(ptrBufCom->buf.cmd.dati);
	uint16_t rispLen = 0;

	rispLen += 1; // aggiungo (parm_rx->param)
	switch(parm_rx->param)
	{
		case 0:
			over_mem.reset_counter = 0;
			break;
		case 1:				// stay in kernel
			if (over_mem.sys_flag.bit.DisableFlash == 1)	//verifica permesso di accesso
			{
				ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_FLASH_DISABLE;
				break;
			}

			//param = 0	=> stay_kernel = 0	& 	reset = 1
			//param = 1	=> stay_kernel = 1	& 	reset = 1
			//param = 2	=> stay_kernel = 0	& 	reset = 0
			//param = 3	=> stay_kernel = 1	& 	reset = 0
			switch (parm_rx->dati[0])
			{
				case 0:
					over_mem.sys_flag.bit.stay_loader = 0;
					over_mem.sys_flag.bit.req_reset = 1;
					break;

				case 1:
					over_mem.sys_flag.bit.stay_loader = 1;
					over_mem.sys_flag.bit.req_reset = 1;
					break;

				case 2:
					over_mem.sys_flag.bit.stay_loader = 0;
					over_mem.sys_flag.bit.req_reset = 0;
					break;

				case 3:
					over_mem.sys_flag.bit.stay_loader = 1;
					over_mem.sys_flag.bit.req_reset = 0;
					break;

				default:
					ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_COP_NOT_SUP;
			}
			break;

		case 2:			// reset kernel
			if (over_mem.sys_flag.bit.DisableFlash == 1)	//verifica permesso di accesso
			{
				ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_FLASH_DISABLE;
				break;
			}

			over_mem.sys_flag.bit.req_reset = 1;
			break;
		case 3:			//imposta Address Scheda
			over_mem.AddressScheda[0] = parm_rx->dati[0];
			over_mem.AddressScheda[1] = parm_rx->dati[1];
			over_mem.AddressScheda[2] = parm_rx->dati[2];
			break;
		case 5:				// set  DisableFlash
			if(parm_rx->dati[0] == 0)
			{
				over_mem.sys_flag.bit.DisableFlash = 0;
			}
			else
			{
				over_mem.sys_flag.bit.DisableFlash = 1;
			}

			break;


		case 6:				// reset ritardato
			over_mem.timeout_ric_reset_5mS = *((uint16_t *)&parm_rx->dati[0]);				// A passi da 5mS
			over_mem.sys_flag.bit.req_jmp_loader_rit = 1;
			break;

		case 7:				// jmp in main ritardato ritardato
			over_mem.timeout_ric_reset_5mS = *((uint16_t *)&parm_rx->dati[0]);				// A passi da 5mS
			over_mem.sys_flag.bit.req_jmp_main_rit = 1;
			break;
		/*
		case 8:				// scrittura del ritardo del "passante" LUNGO
			timeout_ptcl_fw_get_tout_long =  *((uint16_t *)&parm_rx->dati[0]);	// A passi da 5 mS
			break;
		*/

		/* case 0x10:
			//-----	Inizializzazione statistiche
			stats.accu_cycles 			= 0;
			stats.cur_cycles_per_second = 0;
			stats.max_cycles_per_second = 0;
			stats.min_cycles_per_second = 0xFFFFFFFF;
			stats.max_2uS_time_cycle 	= 0;

			systmr_b_5ms.stats = 0;

			break; */
		default:
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_COP_NOT_SUP;
	}
	return(rispLen);
}

/*****************************************************************
	cod_op_0A: Lettura parametrica di kernel
		parametri:
			uint8	param			0 -> over_mem
									1 -> stats
		dati restituiti:
			buffer dati
******************************************************************/
uint16_t  	cod_op_0A(void)
{
	uint16_t rispLen = 0;
	switch (ptrBufCom->buf.cmd.dati[0])
	{
		case 0:			//over_mem
			//memcpy_secure((void *)&(ptrBufCom->buf.cmd.dati[1]), DIMBUF_GES_COM - MAX_COM_OVHD - 1, (void *)&(over_mem), sizeof(over_mem));
			memcpy((void *)&(ptrBufCom->buf.cmd.dati[1]), (void *)&(over_mem), sizeof(over_mem));
			rispLen += sizeof(over_mem) + 1;
			break;
		//case 1:			// stats
		//	memcpy_secure((void *)&(ptrBufCom->buf.cmd.dati[1]), DIMBUF_GES_COM - MAX_COM_OVHD - 1, (void *)&(stats), sizeof(stats));
		//	rispLen += sizeof(stats) + 1;
		//	break;
		default:
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			break;
	}
	return(rispLen);
}
/*****************************************************************
	cod_op_0C: OR memoria su 4 byte adr e len dati
		parametri:
			uint32_t	addr
			uint16_t	len		vale sempre 1
			uint8	dato
		dati restituiti:
			uint32_t	addr
			uint16_t	len
******************************************************************/
uint16_t  	cod_op_0C(void)
{
//	p_cop_04_06_0C_0E_t		*parm_rxtx = (p_cop_04_06_0C_0E_t *)&(ptrBufCom->buf.cmd.dati);
	p_cop_04_06_0C_0E_t		parm_rxtx;
	int8_t					tipo_memoria_start;
	uint16_t rispLen = 0;

	parm_rxtx.addr = ptrBufCom->buf.cmd.dati[0] + (ptrBufCom->buf.cmd.dati[1] << 8) + (ptrBufCom->buf.cmd.dati[2] << 16) + (ptrBufCom->buf.cmd.dati[3] << 24);
	parm_rxtx.len = ptrBufCom->buf.cmd.dati[4] + (ptrBufCom->buf.cmd.dati[5] << 8);

	rispLen += sizeof(parm_rxtx.addr) + sizeof(parm_rxtx.len);

	tipo_memoria_start = msrGetMemType(parm_rxtx.addr);

	if ((tipo_memoria_start == MEM_ID_SRAM) || (tipo_memoria_start == MEM_ID_REG))
	{
		*((uint8_t *)parm_rxtx.addr) |= *((uint8_t *)&(ptrBufCom->buf.cmd.dati[6]));
	}
	else
	{
		// Errore nei dati -> impossibile eseguire richiesta
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
	}
	return(rispLen);
}

/*****************************************************************
	cod_op_0E: AND memoria su 4 byte adr e len dati
		parametri:
			uint32_t	addr
			uint16_t	len		vale sempre 1
			uint8	dato
		dati restituiti:
			uint32_t	addr
			uint16_t	len
******************************************************************/
uint16_t  	cod_op_0E(void)
{
//	p_cop_04_06_0C_0E_t		*parm_rxtx = (p_cop_04_06_0C_0E_t *)&(ptrBufCom->buf.cmd.dati);
	p_cop_04_06_0C_0E_t		parm_rxtx;
	int8_t					tipo_memoria_start;
	uint16_t rispLen = 0;

	parm_rxtx.addr = ptrBufCom->buf.cmd.dati[0] + (ptrBufCom->buf.cmd.dati[1] << 8) + (ptrBufCom->buf.cmd.dati[2] << 16) + (ptrBufCom->buf.cmd.dati[3] << 24);
	parm_rxtx.len = ptrBufCom->buf.cmd.dati[4] + (ptrBufCom->buf.cmd.dati[5] << 8);

	rispLen += sizeof(parm_rxtx.addr) + sizeof(parm_rxtx.len);

	tipo_memoria_start = msrGetMemType(parm_rxtx.addr);

	if ((tipo_memoria_start == MEM_ID_SRAM) || (tipo_memoria_start == MEM_ID_REG))
	{
		*((uint8_t *)parm_rxtx.addr) &= *((uint8_t *)&(ptrBufCom->buf.cmd.dati[6]));
	}
	else
	{
		// Errore nei dati -> impossibile eseguire richiesta
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
	}
	return(rispLen);
}

/*****************************************************************
	cod_op_10: cancellazione flash
		parametri:
			uint32_t	addr
			uint32_t	len
		dati restituiti:
			uint32_t	addr
			uint32_t	len
******************************************************************/
uint16_t  	cod_op_10(void)
{
//	p_cop_10_t				*parm_rxtx = (p_cop_10_t *)&(ptrBufCom->buf.cmd.dati);
	p_cop_10_t				parm_rxtx;
	int8_t					tipo_memoria_start, tipo_memoria_end;
	uint16_t rispLen = 0;

	parm_rxtx.addr = ptrBufCom->buf.cmd.dati[0] + (ptrBufCom->buf.cmd.dati[1] << 8) + (ptrBufCom->buf.cmd.dati[2] << 16) + (ptrBufCom->buf.cmd.dati[3] << 24);
	parm_rxtx.len = ptrBufCom->buf.cmd.dati[4] + (ptrBufCom->buf.cmd.dati[5] << 8) + (ptrBufCom->buf.cmd.dati[6] << 16) + (ptrBufCom->buf.cmd.dati[7] << 24);

	//rispLen += sizeof(p_cop_10_t) - 1;
	rispLen += sizeof(parm_rxtx.addr) + sizeof(parm_rxtx.len);

	tipo_memoria_start = msrGetMemType(parm_rxtx.addr);
	tipo_memoria_end = msrGetMemType(parm_rxtx.addr + parm_rxtx.len - 1);

	if ((tipo_memoria_start == MEM_ID_UNKNOWN)	||
		(tipo_memoria_end == MEM_ID_UNKNOWN)	||
		(tipo_memoria_start != tipo_memoria_end))
	{
		// Zona di memoria con accesso non consentito
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
		return(rispLen);
	}
	if (over_mem.sys_flag.bit.DisableFlash == 1)	//verifica permesso di accesso
	{
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_FLASH_DISABLE;
		return(rispLen);
	}

	switch (tipo_memoria_start)
	{
		case MEM_ID_FLASH:
			if (msrFlashErase(parm_rxtx.addr, parm_rxtx.len) != ERASE_FLASH_OK)
			{
				ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			}
			break;
		default:
			// Errore nei dati -> impossibile eseguire richiesta
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			break;
	}
	return(rispLen);
}
/*****************************************************************
	cod_op_14: calcolo checksum
		parametri:
			uint32_t	addr
			uint32_t	len
		dati restituiti:
			uint32_t	addr
			uint32_t	len
******************************************************************
uint16_t  	cod_op_14(void)
{
	p_cop_14_rx_t		*parm_rx = (p_cop_14_rx_t *)&(ptrBufCom->buf.cmd.dati);
	p_cop_14_tx_t		*parm_tx = (p_cop_14_tx_t *)&(ptrBufCom->buf.cmd.dati);
	int8_t				tipo_memoria_start, tipo_memoria_end;
	uint16_t rispLen = 0;

	rispLen += 2;

	tipo_memoria_start = msrGetMemType(parm_rx->addr_start);
	tipo_memoria_end = msrGetMemType(parm_rx->addr_end);

	switch(parm_rx->param)
	{
		case 0:
			// Lancio della checksum
			if (buf_chk.prenota == CHK_LIBERO)
			{
				if (parm_rx->addr_end <= parm_rx->addr_start)
				{
					// Dati passati non validi
					parm_tx->stato = CHK_KO;				//ack
					break;
				}

				if ((tipo_memoria_start == MEM_ID_UNKNOWN)	||
					(tipo_memoria_end == MEM_ID_UNKNOWN)	||
					(tipo_memoria_start != tipo_memoria_end))
				{
					// Zona di memoria con accesso non consentito
					parm_tx->stato = CHK_KO;				//ack
					//ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
					break;
				}

				buf_chk.addr_start 	= parm_rx->addr_start;
				buf_chk.addr_end 	= parm_rx->addr_end;
				buf_chk.prenota 	= CHK_PRENOTATO;
				parm_tx->stato 		= CHK_PRENOTATO;		//ack
			}
			else
			{
				parm_tx->stato = CHK_IN_CORSO;				//ack
			}
			break;
		case 1:
			// Verifica della checksum
			switch(buf_chk.prenota)
			{
				case CHK_LIBERO:
					// Calcolo checksum non lanciato!
					buf_chk.prenota = CHK_LIBERO;
					parm_tx->stato = CHK_LIBERO;			//ack
					break;
				case CHK_OK:
					// Checksum terminata
					buf_chk.prenota = CHK_LIBERO;
					parm_tx->stato = CHK_OK;				//ack
					parm_tx->checksum = buf_chk.checksum;
					rispLen += 4;
					break;
				default:
					// Checksum in corso
					parm_tx->stato = CHK_IN_CORSO;			//ack
					break;
			}
			break;
		default:
			// Dati passati non validi
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			break;
	}
	return(rispLen);
} ****/


/****************************************************************
	cod_op_16:
	Comandi dedicati per la gestione della lan
****************************************************************/
uint16_t  	cod_op_16(void)
{
	p_cop_16_0_1_rtx_t		*parm_rtx_0_1 = (p_cop_16_0_1_rtx_t *)&(ptrBufCom->buf.cmd.dati);
	p_cop_16_2_7_rtx_t		*parm_rtx_2_7 = (p_cop_16_2_7_rtx_t *)&(ptrBufCom->buf.cmd.dati);

	uint16_t rispLen = 0;
	rispLen += + 1;
	uint32_t offset = 0;

	switch (parm_rtx_0_1->param)
	{

		// Comandi x mac address
		case 0:			// leggi MAC
			parm_rtx_0_1->fabParam = fabParametri;
			rispLen += + sizeof(ethParametri_t);
			break;

		case 1:			// scrivi MAC
			common_flash_to_ram();
			memcpy(param_sbanc_ram, &(parm_rtx_0_1->fabParam), sizeof(ethParametri_t));
			common_ram_to_flash();
			break;

		// Comandi di lettura
		case 2:			// leggi configurazione lan in RAM
			parm_rtx_2_7->lan_conf = commonParametri;
			rispLen += + sizeof(commonParametri_lan_t);
			break;

		case 3:			// leggi configurazione lan banco 1
			parm_rtx_2_7->lan_conf = commonParametri_flash_1;
			rispLen += + sizeof(commonParametri_lan_t);
			break;

		case 4:			// leggi configurazione lan banco 2
			parm_rtx_2_7->lan_conf = commonParametri_flash_2;
			rispLen += + sizeof(commonParametri_lan_t);
			break;

		// Comandi di scrittura
		case 5:			// scrivi configurazione lan in RAM
			commonParametri = parm_rtx_2_7->lan_conf;
			// Occorre attivare la configurazione
			break;

		//case 6:			// scrivi configurazione lan banco 1
		//	msrFlashErase((uint32_t)&commonParametri_flash_1, sizeof(commonParametri_lan_t));
		//	msrFlashWrite_u8((uint32_t)&commonParametri_flash_1, (uint8 *)&(parm_rtx_2_7->lan_conf), sizeof(commonParametri_lan_t));
		//	break;
		case 7: // scrivi configurazione lan banco 2
			//Sbancamento
			common_flash_to_ram();
			offset = sizeof(fabParametri) + sizeof(commonParametri_flash_1);
			memcpy(&param_sbanc_ram[offset], (uint8_t *)&(parm_rtx_2_7->lan_conf), sizeof(parm_rtx_2_7->lan_conf));
			common_ram_to_flash();
			break;
		case 8: // copia configurazione lan banco 2 su lan banco 1
			//Sbancamento
			common_flash_to_ram();
			offset = sizeof(fabParametri);
			memcpy(&param_sbanc_ram[offset], (uint8_t *)&(commonParametri_flash_2), sizeof(commonParametri_flash_2));
			common_ram_to_flash();
			break;
		default:
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			break;
	}

	return(rispLen);
}


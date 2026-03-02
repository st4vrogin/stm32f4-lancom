/******************************************************************************
	@prog		TERMINAL SERVER
	@gruppo		CPU
	@file		AppCodOp.c
	@brief		gestione codici operativi di main

	@author		Siel Tre srl
	@version	0.0.1	2015-01-23
******************************************************************************/

#include "CommonDef.h"
#include "common_data.h"
#include "data.h"
#include "AppCodOp.h"
#include "mem_siel_rtos.h"
#include <string.h>
#include "sbanc.h"
#include "Protocol/Codop.h"
/*****************************************************************
	Variabili extern
******************************************************************/
/*****************************************************************
	Funzioni extern
******************************************************************/
uint8_t	CheckPassword(const char* pwd);

/*****************************************************************
	Funzioni locali
******************************************************************/
uint16_t	cod_op_mancante(void);

uint16_t  	cod_op_20(void);
uint16_t  	cod_op_22(void);
uint16_t  	cod_op_24(void);
uint16_t  	cod_op_28(void);
uint16_t  	cod_op_2A(void);
uint16_t  	cod_op_2C(void);

uint16_t  leggi_dati16(uint8_t *p);
uint32_t  leggi_dati32(uint8_t *p);
void  	scrivi_dati(uint8_t *p, uint32_t address, uint8_t len);

/*****************************************************************
	Costanti locali
******************************************************************/
const cod_op_func_t cod_op_func[] =
{
	{	cod_op_20 },			// 0x20
	{	cod_op_22 },			// 0x22
	{	cod_op_24 },			// 0x24		//Letture parametriche
	{	cod_op_mancante },		// 0x26     //EX RTC ora rimosso
	{	cod_op_28 },			// 0x28		//buffer di ping da remoto
	{	cod_op_2A },			// 0x2A
	{	cod_op_mancante },		// 0x2C
	{	cod_op_mancante },		// 0x2E		//non definiti
	{	cod_op_mancante },		// 0x32		//buffer stato da CPU a TFT
	{	cod_op_mancante },		// 0x34		//CopOp lettura / scrittura stringhe utente
};

/*****************************************************************
	Tabella mappatura dati (per protocollo comunicazione)
******************************************************************/
typedef struct __attribute__((packed))
{
	uint8_t		grpDato;			//codice identificativo del gruppo dati
	uint8_t		codDato;			//codice identificativo del dato
	/*uint32_t*/void *		addrDato;			//address del dato
	uint16_t		lenDato;			// lunghezza del dato
	uint16_t		lenElem; 			// lunghezza di un singolo elemento dell'array ( = len se non è un array)
}MapDati_t;

const	MapDati_t		MapDati[] =
{
//	{0x00, 0x00, (void *)&dato,		sizeof(dato),		sizeof(dato_t)},

	// sistema
	{0x00, 0x40, (void *)&MasterFlag,			sizeof(MasterFlag),				sizeof(MasterFlag_t)},
//	{0x00, 0xA0, (void *)&StatoAutomiMaster,	sizeof(StatoAutomiMaster),		sizeof(AppStatoAutomi_t)},
	{0x01, 0x00, (void *)&stato_IO.in,			sizeof(stato_IO.in),			sizeof(in_L_t)},
	{0x01, 0x20, (void *)&outputDigitali[0],	sizeof(outDig_L_t),				sizeof(outDig_L_t)},
	{0x01, 0x40, (void *)&outputDigitali[1],	sizeof(outDig_L_t),				sizeof(outDig_L_t)},
	{0x01, 0x60, (void *)&stato_IO.inValue,		sizeof(stato_IO.inValue),		sizeof(inValue_t)},
//	{0x01, 0x80, (void *)&stato_IO.outValue,	sizeof(stato_IO.outValue),		sizeof(outValue_t)},
	{0x01, 0xA0, (void *)&sistema,				sizeof(sistema),				sizeof(sistema_t)},
	{0x01, 0xC0, (void *)&debug[0],				sizeof(debug),					sizeof(debug)},
	/* orologi
	{0x02, 0x00, (void *)&MasterTimer_ticks,	sizeof(MasterTimer_ticks),		sizeof(MasterTimer_ticks_t)},
	{0x02, 0x20, (void *)&MasterTimer_u16_1mS,	sizeof(MasterTimer_u16_1mS),	sizeof(MasterTimer_u16_1mS_t)},
	{0x02, 0x40, (void *)&MasterTimer_u16_5mS,	sizeof(MasterTimer_u16_5mS),	sizeof(MasterTimer_u16_5mS_t)},
	{0x02, 0x60, (void *)&MasterTimer_u16_100mS,sizeof(MasterTimer_u16_100mS),	sizeof(MasterTimer_u16_100mS_t)},
	{0x02, 0x80, (void *)&MasterTimer_u16_1S,	sizeof(MasterTimer_u16_1S),		sizeof(MasterTimer_u16_1S_t)},
	{0x02, 0xA0, (void *)&MasterTimer_u32_1S,	sizeof(MasterTimer_u32_1S),		sizeof(MasterTimer_u32_1S_t)},
	// Automi
	{0x0A, 0x00, (void *)&StatoAutomiMaster[0],	sizeof(AppStatoAutomi_t),		sizeof(AppStatoAutomi_t)},
	{0x0A, 0x10, (void *)&StatoAutomiMaster[1],	sizeof(AppStatoAutomi_t),		sizeof(AppStatoAutomi_t)},
	{0x0A, 0x20, (void *)&StatoAutomiMaster[2],	sizeof(AppStatoAutomi_t),		sizeof(AppStatoAutomi_t)},
	{0x0A, 0x30, (void *)&StatoAutomiMaster[3],	sizeof(AppStatoAutomi_t),		sizeof(AppStatoAutomi_t)},
	{0x0A, 0x40, (void *)&StatoAutomiMaster[4],	sizeof(AppStatoAutomi_t),		sizeof(AppStatoAutomi_t)},
	{0x0A, 0x50, (void *)&StatoAutomiMaster[5],	sizeof(AppStatoAutomi_t),		sizeof(AppStatoAutomi_t)}, */

	// parametri
	{0x0B, 0x00, (void *)&fabParametri,				sizeof(fabParametri),				sizeof(ethParametri_t)},
	{0x0B, 0x20, (void *)&commonParametri,			sizeof(commonParametri),			sizeof(commonParametri_lan_t)},
	{0x0B, 0x40, (void *)&commonParametri_flash_1,	sizeof(commonParametri_flash_1),	sizeof(commonParametri_lan_t)},
	{0x0B, 0x60, (void *)&commonParametri_flash_2,	sizeof(commonParametri_flash_2),	sizeof(commonParametri_lan_t)},

	{0x0C, 0x00, (void *)&masterParametri,			sizeof(masterParametri),			sizeof(masterParametri_t)},
	{0x0C, 0x20, (void *)&masterParametri_flash_fab,sizeof(masterParametri_flash_fab),	sizeof(masterParametri_t)},
	{0x0C, 0x40, (void *)&masterParametri_flash,	sizeof(masterParametri_flash),		sizeof(masterParametri_t)},

	{0x10, 0x00, (void *)&buf_ping_tx_0,			sizeof(buf_ping_tx_0),				sizeof(buf_ping_tx_0_t)},

};


/*****************************************************************
	Gestione codOP di main (codOp >= 0x20)
******************************************************************/
uint16_t	ApplCodOp(uint8_t CodOp)
{
	uint8_t  cod_op_index;
	uint16_t rispLen = 0;

	//MasterTimer_u16_1S.refresh_wd_sw = 0;	// Qualunque codice operativo di master azzera il wd_sw

	cod_op_index = (CodOp - 0x20) >> 1;		//0x20 primo cod_op di main
	if (cod_op_index >= sizeof(cod_op_func) / sizeof(cod_op_func_t))
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_COP_NOT_SUP;
	else
		rispLen = cod_op_func[cod_op_index].func();
	return(rispLen);
}

/*****************************************************************
	COD_OP MANCANTE:
	codice operativo non definito
******************************************************************/
uint16_t	cod_op_mancante(void)
{
	ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_COP_NOT_SUP;
	return(0);
}

/****************************************************************
	cod_op_20: versioni Main

		dati restutuiti:
			versione-data Master
****************************************************************/
uint16_t  	cod_op_20(void)
{
//	ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_COP_NOT_SUP;
	uint16_t rispLen = 0;

	switch (ptrBufCom->buf.cmd.dati[0])
	{
		case 0:
			// Versioni Main
			memcpy_secure((void *)&(ptrBufCom->buf.risp.dati[1]), sizeof(ptrBufCom->buf.risp.dati) - 1, (void *)&(MASTER_ver), sizeof(MASTER_ver));
			rispLen += sizeof(MASTER_ver) + 1;																			// +1 perché aggiungo anche il sub_code
			break;
		default:
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			break;
	}

	return(rispLen);
}

/****************************************************************
	cod_op_22:
	Comandi parametrici.
****************************************************************/
uint16_t  	cod_op_22(void)
{
	p_cop_22_rtx_t		*parm_rtx = (p_cop_22_rtx_t *)&(ptrBufCom->buf.cmd.dati);

	uint16_t rispLen = 0;
	rispLen += + 1;

	switch (parm_rtx->param)
	{
		// Comandi di lettura
		case 0:			// leggi configurazione in RAM
			parm_rtx->data.masterParametri = masterParametri;
			rispLen += + sizeof(masterParametri_t);
			break;

		case 1:			// leggi configurazione flash banco fabbrica
			parm_rtx->data.masterParametri = masterParametri_flash_fab;
			rispLen += + sizeof(masterParametri_t);
			break;

		case 2:			// leggi configurazione flash banco attivo
			parm_rtx->data.masterParametri = masterParametri_flash;
			rispLen += + sizeof(masterParametri_t);
			break;

		// Comandi di scrittura
		case 3:			// scrivi configurazione in RAM
			masterParametri = parm_rtx->data.masterParametri;
			// Occorre attivare la configurazione
			masterCmd.bit.rst_spi_com = 1;
			break;

		//case 4:			// scrivi configurazione flash banco fabbrica
		//	siel_flash_erase((uint32_t)&masterParametri_flash_fab, sizeof(masterParametri_t));
		//	siel_flash_program_u8((uint32_t)&masterParametri_flash_fab, (uint8_t *)&(parm_rtx->masterParametri), sizeof(masterParametri_t));
		//	break;

		// case 5: //scrive direttamente in flash quello che viene ricevuto dalla seriale
		// 	break;


		// Sbancamenti
		case 6:			// Copia configurazione ram in flash banco attivo			// configurazione flash banco attivo
#ifdef FLASH_PWD_PROTECTED
			if(CheckPassword(parm_rtx->data.password))
			{
				ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
				break;
			}
#endif
			//app_prm_flash_to_ram();
			memcpy_secure(param_sbanc_ram, sizeof(param_sbanc_ram), &masterParametri, sizeof(masterParametri_t));
			app_prm_ram_to_flash();
			break;

		case 7:			// Copia configurazione flash banco fabbrica in ram
			masterParametri = masterParametri_flash_fab;
			// Occorre attivare la configurazione
			masterCmd.bit.rst_spi_com = 1;
			break;

		case 8:			// Copia configurazione flash banco attivo in ram
			masterParametri = masterParametri_flash;
			// Occorre attivare la configurazione
			masterCmd.bit.rst_spi_com = 1;
			break;
		
		case 9:			// Escludi automi non strettamente necessari (ad es. per sovrascrittura Loader)
		case 10:		// Abilita tutti gli automi 
			MasterFlag.bit.runMinimal = (parm_rtx->param % 2);
			break;


		default:
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			break;
	}

	return(rispLen);
}

/****************************************************************
	cod_op_24:
	letture parametriche
	Restituiscono sia l'indirizzo della struttura (per accesso in scrittura)
	sia il contenuto della struttura stessa. In caso di IMPOSSIBILITA'
	di accesso per "indirizzo", verrà restituito NO_ADDRESS
****************************************************************/
uint16_t  	cod_op_24(void)
{
	uint16_t rispLen = 0;

	switch (ptrBufCom->buf.cmd.dati[0])
	{
		/* case 1:			// lettura StatoAutomiMaster
			scrivi_dati(&ptrBufCom->buf.cmd.dati[1], (uint32_t)&StatoAutomiMaster, 4);
			memcpy_secure(&ptrBufCom->buf.cmd.dati[5], DIMBUF_GES_COM -MAX_COM_OVHD - 5, &StatoAutomiMaster, sizeof(StatoAutomiMaster));
			rispLen += + 1 + 4 + sizeof(StatoAutomiMaster);
			break;
		*/

		case 2:			// lettura MasterFlag
			scrivi_dati(&ptrBufCom->buf.cmd.dati[1], (uint32_t)&MasterFlag, 4);
			memcpy_secure(&ptrBufCom->buf.cmd.dati[5], DIMBUF_GES_COM -MAX_COM_OVHD - 5,(void *)&MasterFlag, sizeof(MasterFlag));
			rispLen += + 1 + 4 + sizeof(MasterFlag);
			break;
		case 0:			// lettura RTC (non supportato)
		case 1: 		// lettura StatoAutomiMaster (non supportato)
		default:
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			break;
	}
	return(rispLen);
}


/****************************************************************
	cod_op_28:
	Codice unico di scambio dati tra la scheda TS e PC
****************************************************************/
uint16_t  	cod_op_28(void)
{
	uint16_t 				rispLen = 0;
	uint8_t				tipo_ping = ptrBufCom->buf.cmd.dati[0];

	rispLen += 1 ;	// Dimensione + tipo

	switch (tipo_ping)
	{
		// i primi 0x10 tipi sono dedicati a possibili futuri buffer pi ping da spedire alla centrale
		case 0x00:			//
		{
			p_cop_28_0_tx_0_t	*p_cop_28_0_tx_0 = (p_cop_28_0_tx_0_t *)&(ptrBufCom->buf.cmd.dati[1]);

			p_cop_28_0_tx_0->buf_ping_tx_0 = buf_ping_tx_0;
			rispLen += sizeof(p_cop_28_0_tx_0_t);	// Dimensione + struttura
			break;
		}

		// Dal 0x10 iniziano i sottocodici dedicati al buffer di scrittura da parte della centrale
		case 0x10:			//
		{
			p_cop_28_10_rx_0_t	*p_cop_28_10_rx_0 = (p_cop_28_10_rx_0_t *)&(ptrBufCom->buf.cmd.dati[1]);

			if (buf_ping_tx_0.flags.bit.disable_ping_rx == 0)
			{
				buf_ping_rx_0 = p_cop_28_10_rx_0->buf_ping_rx_0;
				
				buf_ping_tx_0.flags.bit.lan_manuale = buf_ping_rx_0.cmd_ric.bit.lan_manuale;
	
				// Aggiorno tutti gli output
				if (buf_ping_rx_0.cmd_ric.bit.lan_manuale)
				{
					OUT_RL_SCAMBIO_LAN_TS	= buf_ping_rx_0.cmd_pin_out.bit.rl_scambio_lan_ts;
					OUT_RL_SCAMBIO_LAN_VOIP	= buf_ping_rx_0.cmd_pin_out.bit.rl_scambio_lan_voip;
				}
				
				OUT_RL_SCAMBIO_BF_VOIP	= buf_ping_rx_0.cmd_pin_out.bit.rl_scambio_bf_voip;
				OUT_RL_SCAMBIO_N_R		= buf_ping_rx_0.cmd_pin_out.bit.rl_scambio_n_r;
				OUT_BKF_SCAMBIO_NR_BB   = buf_ping_rx_0.cmd_pin_out.bit.rl_scambio_n_r;
				CMD_DSP_ON				= buf_ping_rx_0.cmd_pin_out.bit.cmd_dsp_on;
				OUT_CMD_PWD_543			= buf_ping_rx_0.cmd_pin_out.bit.cmd_pwd_543;
				OUT_CMD_RST_uP_543		= buf_ping_rx_0.cmd_pin_out.bit.cmd_rst_up_543;
				OUT_CMD_MST_RESET_BKF	= buf_ping_rx_0.cmd_pin_out.bit.cmd_mst_reset_bkf;
	
				OUT_CMD_EN_ALIM_BKF		= buf_ping_rx_0.cmd_pin_out.bit.cmd_en_alim_bkf;
				OUT_REAR_2				= buf_ping_rx_0.cmd_pin_out.bit.out_rear_2;
				OUT_REAR_1				= buf_ping_rx_0.cmd_pin_out.bit.out_rear_1;
	
				// Analisi comandi a bit
				if (buf_ping_rx_0.cmd_ric.bit.agg_refresh_wd_ram_flash)
				{
					buf_ping_rx_0.cmd_ric.bit.agg_refresh_wd_ram_flash = 0;
					// Aggiorno il valore sia in ram che in flash
					masterParametri.timeout_refresh_wd = buf_ping_rx_0.timeout_refresh_wd;
					memcpy_secure(param_sbanc_ram, sizeof(param_sbanc_ram), &masterParametri, sizeof(masterParametri_t));
					app_prm_ram_to_flash();
				}
				if (buf_ping_rx_0.cmd_ric.bit.agg_refresh_wd_ram)
				{
					masterParametri.timeout_refresh_wd = buf_ping_rx_0.timeout_refresh_wd;
				}
	
				if (buf_ping_rx_0.cmd_ric.bit.azzera_num_reset)
				{
					// Richiesta di azzeramento numero di reset
					over_mem.reset_counter = 0;
				}
			}
			else
			{
				ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_PORTA_BUSY;			
			}
			break;
		}
		
		// Sezioni comandi dedicati
		case 0x20:
		{
			// Attivazione / disattivazione disable_ping_rx
			p_cop_28_20_rx_0_t		*p_cop_28_20_rx_0 = (p_cop_28_20_rx_0_t *)&(ptrBufCom->buf.cmd.dati[1]);
			
			//MasterTimer_u16_1S.timeout_disable_ping_rx = 0;
			
			buf_ping_tx_0.flags.bit.disable_ping_rx 	= p_cop_28_20_rx_0->disable_ping_rx;
			
			
			if (p_cop_28_20_rx_0->timeout_disable_ping_rx > 0)
			{
				//timeout_disable_ping_rx 				= p_cop_28_20_rx_0->timeout_disable_ping_rx;
			}

			if ((p_cop_28_20_rx_0->disable_ping_rx == 0) || (p_cop_28_20_rx_0->timeout_disable_ping_rx == 0))
			{
				//timeout_disable_ping_rx 				= TIMEOUT_DISABLE_PING_RX_1S;
			}
			break;

		}

		default:
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			break;
	}

	return(rispLen);
}

/****************************************************************
	cod_op_2A:
	dati:
		[0]	grpDato		1 byte: codice identificativo del gruppo dati
		[1] codDato 	1 byte: codice del dato richiesto
		[2] qualeElem	2 byte: n. dell'elemento richiesto (FFFF se non è un array)

	restituisce per il dato richiesto:
		[0] grpDato		1 byte: codice identificativo del gruppo dati
		[1] codDato 	1 byte: codice del dato richiesto
		[2] qualeElem	2 byte: n. dell'elemento richiesto (FFFF se non è un array)
		[4] addrDato 	4 byte: address del dato
		[8] lenDato		2 byte: lunghezza del dato
		[10] nElem		2 byte: n.elementi presenti (se è un array > 1)
		[12]lenElem 	2 byte: lunghezza di un singolo elemento dell'array ( = len se non è un array)
****************************************************************/
uint16_t  	cod_op_2A(void)
{
	uint16_t rispLen = 0;
	uint8_t grpDato = ptrBufCom->buf.cmd.dati[0];
	uint8_t codDato = ptrBufCom->buf.cmd.dati[1];
	uint16_t qualeElem = leggi_dati16(&ptrBufCom->buf.cmd.dati[2]);
	uint16_t MapDatiNum = sizeof(MapDati)/sizeof(MapDati_t);
	uint16_t curDato;
	uint16_t nElem;

		//controlla congruenza dati:
		//	- codDato deve essere presente in tabella
		//	- qualeElem < nElem presenti per il dato corrente oppure 0xFF = intera tabella
	for (curDato = 0; curDato < MapDatiNum; curDato++)
		if ((MapDati[curDato].grpDato == grpDato) && (MapDati[curDato].codDato == codDato)) break;

	if (curDato >= MapDatiNum)
	{
		ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
		return(rispLen);
	}

	nElem = MapDati[curDato].lenDato / MapDati[curDato].lenElem;
	if(nElem == 1)
	{
		qualeElem = 0xFFFF;
	}
	else
	{
		if ((qualeElem >= nElem) && (qualeElem != 0xFFFF))
		{
			ptrBufCom->buf.risp.flag.bit.ack = PTCL_ERR_DATI;
			return(rispLen);
		}
	}

		// scrive risposta
	if (qualeElem == 0xFFFF)
	{
			//ritorna address dell'intero dato
		scrivi_dati((uint8_t *)&ptrBufCom->buf.cmd.dati[4],(uint32_t) MapDati[curDato].addrDato,4);
		scrivi_dati((uint8_t *)&ptrBufCom->buf.cmd.dati[8], (uint32_t) MapDati[curDato].lenDato,2);
		scrivi_dati((uint8_t *)&ptrBufCom->buf.cmd.dati[10], (uint32_t) nElem,2);
		scrivi_dati((uint8_t *)&ptrBufCom->buf.cmd.dati[12],(uint32_t)  MapDati[curDato].lenElem,2);
	}
	else
	{
			//ritorna address dell'elemento specificato
		scrivi_dati((uint8_t *)&ptrBufCom->buf.cmd.dati[4],(uint32_t) ((uint32_t)(MapDati[curDato].addrDato) + qualeElem * MapDati[curDato].lenElem) ,4);			//
		scrivi_dati((uint8_t *)&ptrBufCom->buf.cmd.dati[8], (uint32_t) MapDati[curDato].lenElem,2);
		scrivi_dati((uint8_t *)&ptrBufCom->buf.cmd.dati[10], 1,2);
		scrivi_dati((uint8_t *)&ptrBufCom->buf.cmd.dati[12],(uint32_t)  MapDati[curDato].lenElem,2);
	}
	rispLen += + 4 + 10;	  //4:grpDato+codDato+nElem(2)   10:risposta
	return(rispLen);
}


/****************************************************************
****************************************************************/
/****************************************************************
	Routine varie
****************************************************************/
/****************************************************************
****************************************************************/

/****************************************************************
	Lettura di un dato su piu' byte da un buffer
****************************************************************/
uint16_t  leggi_dati16(uint8_t *p)
{
	return(p[0] + (p[1] << 8));
}

uint32_t  leggi_dati32(uint8_t *p)
{
	return(p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24));
}

/****************************************************************
	Restituzione address della struttura richiesta
****************************************************************/
void  	scrivi_dati(uint8_t *p, uint32_t address, uint8_t len)
{
	uint8_t	i;
	for (i = 0 ; i < len ; i++)
	{
		p[i] = (uint8_t)(address >> (i*8));
	}
}

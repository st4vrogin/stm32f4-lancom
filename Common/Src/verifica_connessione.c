/*****************************************************************

	Project: VoIP SRC Terminal Server

	Module: verifica_connessione.c
	Created: 2015/09/01
	Descr:

******************************************************************/

#include "macros.h"

#include "data.h"
#include "common_data.h"

#include "verifica_connessione.h"
#include "PingHlp.h"
#include <string.h>
#include "cmsis_os2.h"
#include "FreeRTOS_IP.h"
#include "lan_init.h"


/*****************************************************************
	Definizioni
******************************************************************/
AppStatoAutomi_t				automaVerificaGw;
#define ST_CORRENTE				automaVerificaGw.statoCorrente
#define ST_RITORNO	 			automaVerificaGw.statoRitorno
#define FLAG_STATO				automaVerificaGw.flag.bit.subStato


#define OROL_KVF_100mS						MasterTimer_u16_100mS.verifica_connessione
#define OROL_LAN_STAT_100mS					MasterTimer_u16_100mS.lan_stat

#define	TIMEOUT_VERIFICA_BREVE_100mS	50
#define	TIMEOUT_AUTONEGOTIATION_100mS	450  //Per dare la possibilita ai router cisco di autonegoziare la larghezza di banda
#define	TIMEOUT_VERIFICA_LUNGO_100mS	600
#define	TIMEOUT_VERIFICA_KO_PING_100mS	100
#define TIMEOUT_RACCOLTA_STATISTICHE	(10 * 60) //100mS * x raccoglie statistiche

#define POS_RELE_OVERMEM				over_mem.sys_flag.bit.rele_ts_pos
#define THIS_STACK_SIZE					768

/*****************************************************************
	Funzioni Extern
******************************************************************/
extern	struct stats_		lwip_stats;

/*****************************************************************
	Funzioni Locali
******************************************************************/
void	watchdog_sw(void);

/*****************************************************************
	Costanti  locali
******************************************************************/
const void_func StatiVerificaConnessione[] =
{
	AutoVF_ST_VF_INIT,			// 0
	AutoVF_ST_VF_WAIT,			// 1
	AutoVF_ST_VF_EXEC_PING,		// 2
	AutoVF_ST_VF_VERIFICA_PING,	// 3
	AutoVF_ST_VF_SCAMBIA,			// 4
	AutoVF_ST_VF_WAIT_SCAMBIA,	// 5
	AutoVF_ST_VF_RICONFIGURA_LAN,	// 6
};

const osThreadAttr_t verificaConnessione_attributes = {
  .name = "verifConn",
  .stack_size = THIS_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal
};

/*****************************************************************
	Variabili extern
******************************************************************/

#define	LAN_READY							stato_lan.fields.lan_ready
#define	LAN_RECONF							stato_lan.fields.reconf_request
/*****************************************************************
	Variabili
******************************************************************/
uint16_t	timeout_verifica_connessione;
uint8_t	attesa_riconfigurazione_lan;
uint8_t	posizione_connessione;

/***********************************************************************
************************************************************************

	Automa

***********************************************************************
***********************************************************************/
void	taskVerificaConnessione(void* pvArguments)
{
	for(;;)
	{
		osDelay(1);
		LAN_READY = (uint8_t)FreeRTOS_IsNetworkUp();
		watchdog_sw();

		if (buf_ping_tx_0.flags.bit.disable_ping_rx == 1)
		{
			if (MasterTimer_u16_1S.timeout_disable_ping_rx >= timeout_disable_ping_rx)
			{
				buf_ping_tx_0.flags.bit.disable_ping_rx = 0;
			}
		}
		else
		{
			MasterTimer_u16_1S.timeout_disable_ping_rx = 0;
		}

#ifdef NETSTACK_HISTORICAL_STATS
#if NETSTACK_HISTORICAL_STATS
		if(OROL_LAN_STAT_100mS >= TIMEOUT_RACCOLTA_STATISTICHE)
		{
			OROL_LAN_STAT_100mS = 0;
			if(log_stat_attivo)
			{
				if(log_stat_counter >= MAX_ELEM_VET_STAT)
					log_stat_counter = 0;
				memcpy(&lwip_vettore_storico[log_stat_counter++], &lwip_stats, sizeof(lwip_stats));
			}
		}
#endif
#endif
	
		if (ST_CORRENTE > LAST_ST_VF)	ST_CORRENTE = 0;

		if (buf_ping_rx_0.cmd_ric.bit.lan_manuale == 0)
		{
			StatiVerificaConnessione[ST_CORRENTE]();
		}
		else
		{
			ST_CORRENTE = ST_VF_INIT;
			FLAG_STATO = ENTRA_STATO;
		}
	}
}

/*******************************************************************************
	AUTOMA:			AutomaVerificaConnessione
	STATO:			AutoVF_ST_VF_INIT
	DESCRIZIONE:	Inizializzazione del sistema.
					Per ora non fa nulla. Stato di sola transizione
*******************************************************************************/
void	AutoVF_ST_VF_INIT(void)
{
	GPIO_PinState	stato_rele;
	if (FLAG_STATO == ENTRA_STATO)
	{
		FLAG_STATO = EXEC_STATO;
		stato_rele = HAL_GPIO_ReadPin(RELE_SCAMBIO_LAN_TS_GPIO_Port, RELE_SCAMBIO_LAN_TS_Pin);
		if(stato_rele != POS_RELE_OVERMEM)
		{
			OUT_RL_SCAMBIO_LAN_TS = POS_RELE_OVERMEM;
		}
		OROL_KVF_100mS = 0;
		attesa_riconfigurazione_lan = 0;
	}


	if (OROL_KVF_100mS >= 10)
	{
		// Allinizio occorre una verifica attendendo la fine dell'autonegoziazione
		timeout_verifica_connessione = TIMEOUT_AUTONEGOTIATION_100mS;				//TIMEOUT_AUTONEGOTIATION_100mS;
		posizione_connessione = 0;

		ST_CORRENTE = ST_VF_WAIT;
		FLAG_STATO = ENTRA_STATO;
	}
}

/*******************************************************************************
	AUTOMA:			AutomaVerificaConnessione
	STATO:			AutoVF_ST_VF_WAIT
	DESCRIZIONE:	Stato di attesa prossima verifica
*******************************************************************************/
void	AutoVF_ST_VF_WAIT(void)
{
	if (FLAG_STATO == ENTRA_STATO)
	{
		FLAG_STATO = EXEC_STATO;
		OROL_KVF_100mS = 0;
	}

	if ((OROL_KVF_100mS >= timeout_verifica_connessione) && (LAN_READY))
	{
		ST_CORRENTE = ST_VF_EXEC_PING;
		FLAG_STATO = ENTRA_STATO;
	}
}

/*******************************************************************************
	AUTOMA:			AutomaVerificaConnessione
	STATO:			AutoVF_ST_VF_EXEC_PING
	DESCRIZIONE:	Attivo automa ping su configurazione corrente
*******************************************************************************/
void	AutoVF_ST_VF_EXEC_PING(void)
{
	if (FLAG_STATO == ENTRA_STATO)
	{
		FLAG_STATO = EXEC_STATO;
	}

	// esegui_ping

	start_ping((uint8_t*)&commonParametri.gw_addr[0], 5);

	ST_CORRENTE = ST_VF_VERIFICA_PING;
	FLAG_STATO = ENTRA_STATO;
}

/*******************************************************************************
	AUTOMA:			AutomaVerificaConnessione
	STATO:			AutoVF_ST_VF_VERIFICA_PING
	DESCRIZIONE:	Stato di attesa fine automa ping
*******************************************************************************/
void	AutoVF_ST_VF_VERIFICA_PING(void)
{
	if (FLAG_STATO == ENTRA_STATO)
	{
		FLAG_STATO = EXEC_STATO;
		OROL_KVF_100mS = 0;
	}

	if (ping_report.report_ok == 1)
	{
		// Automa ping ha terminato
		// Verifico il risultato
		//if ((ping_report.pkt_sent == 5) && (ping_report.pkt_loss < 5 / 2))
		if ((ping_report.pkt_sent == 5) && (ping_report.pkt_loss < 2))
		{
			// Risultato OK
			timeout_verifica_connessione = TIMEOUT_VERIFICA_LUNGO_100mS;

			ST_CORRENTE = ST_VF_WAIT;
			FLAG_STATO = ENTRA_STATO;
		}
		else
		{
			// Connessione LAN con problemi
			// Provo a scambiare
			ST_CORRENTE = ST_VF_SCAMBIA;
			FLAG_STATO = ENTRA_STATO;
		}

		stop_ping();
	}

	if (OROL_KVF_100mS >= TIMEOUT_VERIFICA_KO_PING_100mS)
	{
		// L'automa ping non completa l'operazione???
		ST_CORRENTE = ST_VF_SCAMBIA;
		FLAG_STATO = ENTRA_STATO;
	}
}

/*******************************************************************************
	AUTOMA:			AutomaVerificaConnessione
	STATO:			AutoVF_ST_VF_SCAMBIA
	DESCRIZIONE:	Problemi di comunicazione sulla linea in corso -> scambio

					0:	banco 1 rele off
					1:	banco 2 rele off
					2:	banco 1 rele on
					3:	banco 2 rele on
*******************************************************************************/
void	AutoVF_ST_VF_SCAMBIA(void)
{
	int8_t			stato_parametri;
	GPIO_PinState	stato_rele;
	GPIO_PinState	stato_rele_voip;
	uint8_t			rele_scambiato = 0;
	if (FLAG_STATO == ENTRA_STATO)
	{
		FLAG_STATO = EXEC_STATO;
	}

	stato_parametri = memcmp(&commonParametri_flash_1, &commonParametri_flash_2, sizeof(commonParametri_lan_t));
	stato_rele = HAL_GPIO_ReadPin(RELE_SCAMBIO_LAN_TS_GPIO_Port, RELE_SCAMBIO_LAN_TS_Pin);
	stato_rele_voip = HAL_GPIO_ReadPin(RELE_SCAMBIO_LAN_VOIP_GPIO_Port, RELE_SCAMBIO_BF_VOIP_Pin);

	switch(posizione_connessione)
	{
		case 0:
			if (stato_parametri != 0)
			{
				posizione_connessione = 1;
				commonParametri = commonParametri_flash_2;
			}
			else
			{
				posizione_connessione = 2;
				commonParametri = commonParametri_flash_1;
			}
			break;
		case 1:
			posizione_connessione = 2;
			commonParametri = commonParametri_flash_1;
			break;
		case 2:
			if (stato_parametri != 0)
			{
				posizione_connessione = 3;
				commonParametri = commonParametri_flash_2;
			}
			else
			{
				posizione_connessione = 0;
				commonParametri = commonParametri_flash_1;
			}
			break;
		case 3:
		default:
			posizione_connessione = 0;
			commonParametri = commonParametri_flash_1;
			break;
	}

	// Scambio il rele
	if (posizione_connessione < 2)
	{
		if (stato_rele != GPIO_PIN_RESET)
		{
			printf("Scambio relay TS 1 -> 0\n");
			//HAL_GPIO_WritePin(OUT_RL_SCAMBIO_LAN_TS_PORT, OUT_RL_SCAMBIO_LAN_TS_PIN, GPIO_PIN_RESET);
			OUT_RL_SCAMBIO_LAN_TS = 0;
			POS_RELE_OVERMEM = 0;
			rele_scambiato = 1;
		}
		if (stato_rele_voip != GPIO_PIN_RESET)
		{
			OUT_RL_SCAMBIO_LAN_VOIP = 0;
		}
	}
	else
	{
		if (stato_rele != GPIO_PIN_SET)
		{
			printf("Scambio relay TS 0 -> 1\n");
			//HAL_GPIO_WritePin(OUT_RL_SCAMBIO_LAN_TS_PORT, OUT_RL_SCAMBIO_LAN_TS_PIN, GPIO_PIN_SET);
			OUT_RL_SCAMBIO_LAN_TS = 1;
			POS_RELE_OVERMEM = 1;
			rele_scambiato = 1;
		}

		if (stato_rele_voip != GPIO_PIN_SET)
		{
			OUT_RL_SCAMBIO_LAN_VOIP = 1;
		}
	}

	if (rele_scambiato == 0)
	{
		ST_CORRENTE = ST_VF_RICONFIGURA_LAN;
	}
	else
	{
		ST_CORRENTE = ST_VF_WAIT_SCAMBIA;
	}

	buf_ping_tx_0.num_switch_rete++;

	FLAG_STATO = ENTRA_STATO;
}

/*******************************************************************************
	AUTOMA:			AutomaVerificaConnessione
	STATO:			AutoVF_ST_VF_WAIT_SCAMBIA
	DESCRIZIONE:	Stato di attesa scambio rele
*******************************************************************************/
void	AutoVF_ST_VF_WAIT_SCAMBIA(void)
{
	if (FLAG_STATO == ENTRA_STATO)
	{
		FLAG_STATO = EXEC_STATO;
		OROL_KVF_100mS = 0;
	}

	if (OROL_KVF_100mS >= 40)
	{
		//
		ST_CORRENTE = ST_VF_RICONFIGURA_LAN;
		FLAG_STATO = ENTRA_STATO;
	}
}

/*******************************************************************************
	AUTOMA:			AutomaVerificaConnessione
	STATO:			AutoVF_ST_VF_RICONFIGURA_LAN
	DESCRIZIONE:	Stato di riconfigurazione parametri di lan
*******************************************************************************/
void	AutoVF_ST_VF_RICONFIGURA_LAN(void)
{
	if (FLAG_STATO == ENTRA_STATO)
	{
		FLAG_STATO = EXEC_STATO;
		OROL_KVF_100mS = 0;
	}

	if((LAN_RECONF == 0) && (attesa_riconfigurazione_lan == 0))
	{
		printf("Riconfigurazione LAN\n");
		LAN_RECONF = 1;
		attesa_riconfigurazione_lan = 1;
		liFreeRtosIpReinit();
	}

	if(OROL_KVF_100mS >= TIMEOUT_AUTONEGOTIATION_100mS)
	{
		LAN_RECONF = 0;
		// Al prossimo giro eseguo il test con timeout breve
		timeout_verifica_connessione = TIMEOUT_VERIFICA_BREVE_100mS;
		attesa_riconfigurazione_lan = 0;
		ST_CORRENTE = ST_VF_WAIT;
		FLAG_STATO = ENTRA_STATO;
	}
}

/*******************************************************************************
*******************************************************************************/
/*******************************************************************************
	FUNZIONE:		verifica_lan_com
	DESCRIZIONE:	Verifica quali porte lan com sono effettivamente connesse
*******************************************************************************/
void	verifica_lan_com(void)
{
	uint8_t	i;
	buf_ping_tx_0.stato_lan_com.word = 0;

	for (i = 0 ; i < NUM_PORTE_COM ; i++)
	{
		if(LancomClients[i] != FREERTOS_INVALID_SOCKET && LancomClients[i]->u.xTCP.eTCPState == eESTABLISHED)
		{
			buf_ping_tx_0.stato_lan_com.word |= 1<<i;
		}
	}
}

/*******************************************************************************
	FUNZIONE:		watchdog_sw
	DESCRIZIONE:	Watchdog software.
					Verrifica se è presente la connessione con la centrale
*******************************************************************************/
void	watchdog_sw(void)
{
	// Aggiornamento buffer di ping
	if ((masterParametri.timeout_refresh_wd == 0) || (masterParametri.timeout_refresh_wd == 0xFFFF))
		buf_ping_tx_0.timeout_refresh_wd = TIMEOUT_WD_SW_NORMAL;
	else	// Valore consentito
		buf_ping_tx_0.timeout_refresh_wd = masterParametri.timeout_refresh_wd;

	buf_ping_tx_0.reset_counter = over_mem.reset_counter;

	verifica_lan_com();

	if (MasterTimer_u16_1S.start_wd_sw < TIMEOUT_START_WD_SW)
	{
		// Nei primi minuti dall'accensione, il "watchdog_sw" è disattivo
		MasterTimer_u16_1S.refresh_wd_sw = 0;
	}
	else
	{
		// Watchdog_sw attivo
		MasterTimer_u16_1S.start_wd_sw = TIMEOUT_START_WD_SW;	// Evito l'overflow dell'orologio

		// Questo timer viene azzerato dal qualunque codice operativo di master
		if (MasterTimer_u16_1S.refresh_wd_sw > buf_ping_tx_0.timeout_refresh_wd)
		{
			printf("RESET WATCHDOG SW\n");
			// E' scattato il timeout -> la centrale non è più collegata al TerminalServer?
			// Diamo un colpo di reset al sistema
			buf_ping_tx_0.reset_wd_sw++;
			over_mem.sys_flag.bit.req_reset = 1;
		}
	}

	if (buf_ping_rx_0.cmd_ric.bit.reset_ts)
	{
		// Ricevuta richiesta di reset ritardato
		if (MasterTimer_u16_100mS.ritardo_reset > 10)
		{
			over_mem.sys_flag.bit.req_reset = 1;
		}
	}
	else
	{
		MasterTimer_u16_100mS.ritardo_reset = 0;
	}
}

/**
 * @brief Routine di creazione del thread FreeRTOS
 *
 * Va richiamata in main e riportata in tasks.h
 */
osThreadId_t createTask_VerificaConnessione(void* arg)
{
	return osThreadNew(taskVerificaConnessione, arg, &verificaConnessione_attributes);
}

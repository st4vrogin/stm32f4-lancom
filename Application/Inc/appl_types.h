/******************************************************************************
	@prog		TERMINAL SERVER
	@file		ApplicationType.h
	@brief		Strutture dati di applicazione

	@author		SielTre srl
	@version	1.0.0	2023-06-15
******************************************************************************/

#ifndef APPLICATION_INC_APPL_TYPES_H
#define APPLICATION_INC_APPL_TYPES_H

#include <stdint.h>
#include "defs.h"
#include "CommonDef.h"
//#include "MasterDefine.h"
//#include "GestIO.h"



typedef union
{
	uint8_t		byte[4];
	struct
	{
			uint8_t	initDone				:1,		// inizializzazione effettuata
	 				tick_interrupt_rdin		:1,		//
	 				tick_interrupt_wrout	:1,		//
					b0_5					:1,		//
					simInput				:1,		// input simulati
					manOutput				:1,		// gestione Manuale degli output (si ignora l'impostazione degli automi)
					runMinimal				:1,		// se 1 esclude gli automi non necessari (es. LAN-COM)
	    			rst_spi_com_in_corso	:1;		//

			uint8_t	warning_MT				:1,		// preavviso mancanza tensione
	 				save					:1,		// flag salvataggio da effettuare
	 				restore					:1,		// flag restore da effettuare
	    			usaBSRAM				:1,		// flag utilizzo BRAM per save dati macchina
	    			b1_4					:1,		//
	 				b1_5					:1,		//
	 				b1_6					:1,		//
	 				b1_7					:1;		//
		
			uint8_t	modo_man				:1,		// macchina in manuale
	 				modo_ato				:1,		// macchina in automatico
					b2_2					:1,		//
					b2_3					:1,		//
	 				b2_4					:1,		//
	 				b2_5					:1,		//
	 				b2_6					:1,		//
	 				b2_7					:1;		//
		
			uint8_t	b3_0					:1,		//
	 				b3_1					:1,		//
					b3_2					:1,		//
					b3_3					:1,		//
	 				b3_4					:1,		//
	 				b3_5					:1,		//
					b3_6					:1,		//
					b3_7					:1;		//
	}
	bit;
}MasterFlag_t;


/*****************************************************************
	AUTOMI
******************************************************************/
#define ENTRA_STATO		1
#define ESCI_STATO		2
#define EXEC_STATO		3

typedef union __attribute__((packed))
{
    uint8_t		byte;
    struct __attribute__((packed))
	{
				uint8_t	subStato		:2,		// entra stato = 1  esci stato = 2
				b2								:1,
				b3								:1,
				avvio							:1,
				b5								:1,
				b6								:1,
				b7								:1;
	}bit;
} AppAutomiFlag_t;

typedef union __attribute__((packed))
{
    struct __attribute__((packed))
	{
				uint16_t	MT							:1,		//MANCANZA TENSIONE
						ALR							:1,		//ALLARME
						REQ_OFF						:1,		//Richiesta OFF
						REQ_SETUP					:1,		//Richiesta SETUP
						REQ_COLL					:1,		//Richiesta COLLAUDO
						REQ_AVVIO					:1,		//AVVIO CICLO
						REQ_SOSP					:1,		//SOSPENSIONE CICLO
						REQ_RESET					:1,		//RESET
						REQ_ON						:1,		//Richiesta ON
						REQ_SETUP_K					:1,		//Richiesta setup sonda conducibilita'
						TBD							:6;		// da definire
	}bit;

   	uint16_t		w;
} eventi_t;

typedef struct __attribute__((packed))
{
	uint8_t						statoCorrente;	//stato corrente automa
	uint8_t						statoRitorno;		//stato al ritorno dalla sospensione
	uint8_t						statoNext;			//prossimo stato
	AppAutomiFlag_t	flag;						//
	eventi_t 				eventi;					//
} AppStatoAutomi_t;


/***************************************************************************************
	I/O
***************************************************************************************/
typedef union
{
	uint16_t		AD[NUM_ADC];
}inValue_t;

typedef struct
{
 	word_bits_t porta[IO_PORTS];		//define degli input con corrispondenza diretta con porte
}in_L_t;		//INPUT DIGITALI LOGICI

typedef struct
{
 	word_bits_t porta[IO_PORTS];		//define degli output con corrispondenza diretta con porte
}outDig_L_t;	//OUTPUT DIGITALI LOGICI

typedef struct
{
	in_L_t			in;						// stato ingressi digitali
	inValue_t 	inValue;			// valore degli ingressi analogici
	outDig_L_t	*ptOutDig;		// pt stato uscite digitali
}stato_IO_t;



/*****************************************************************
	STATO E PARAMETRI IN FLASH
******************************************************************/
typedef struct
{
	uint16_t 			SaveContestoOK;
	uint16_t			numOFF;
} infoMacchina_t;

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
			uint8_t	cc_default_to_ram			:1,		// richiesta di copia configurazione da flash default a RAM
	 		cc_flash_to_ram							:1,		// richiesta di copia configurazione da flash a RAM
	 		cc_ram_to_flash							:1,		// richiesta di copia configurazione da RAM a flash
			b0_3												:1,		//
			b0_4												:1,		//
			b0_5												:1,		//
			b0_6												:1,		//
	    	rst_spi_com									:1;		//
	}
	bit;
} masterCmd_t;


/*****************************************************************
	STRUTTURE APPLICAZIONE
******************************************************************/
typedef struct __attribute__((packed))
{
	uint16_t		cod_err;														// codice dell'errore sugli automi di alto livello
	uint16_t		err_radice;													// codice del primo errore insorto
	uint8_t			riserva;														// byte di pareggio
	uint16_t		max_sched;													// max tempo di schedulazione
	uint8_t			stati_max_sched[MAX_AUTOMI_MASTER];	// stati automi al max tempo sched
} sistema_t;

/*****************************************************************
	Gestione code
******************************************************************/
typedef struct
{
	uint32_t			testa;
	uint32_t			coda;
}code_t;

/*****************************************************************
	PARAMETRI
******************************************************************/
typedef struct __attribute__((packed))
{
	uint32_t	uart_speed;
	uint8_t	n_bit;
	uint8_t	parity;
	uint8_t	nbit_stop;
}cfg_uart_t;

typedef struct __attribute__((packed))
{
	uint16_t		porta_servizio;

	uint8_t		usa_carattere_per_tx;		// Se = 0 -> non usato
	uint8_t		carattere_per_tx;			// Quando ricevo un dato sulla seriale, se uguale a usa_carattere_per_tx
											// inoltro immediatamente il pacchetto sulla lan

	uint16_t		timeout_send_tx;			// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan

	uint16_t		len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
										// inoltro immediatamente il messaggio sulla lan
	uint16_t		write_timeout;			// Se > 0 indica il tempo massimo in secondi senza scambio di dati per cui la connessione TCP debba rimanere attiva
} param_lan_com_t;

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
				uint8_t	rl_scambio_n_r_def_val		:1,		// valore di default dello scambio N/R
		 				b0_1						:1,		//
		 				b0_2						:1,		//
						b0_3						:1,		//
						b0_4						:1,		//
						b0_5						:1,		//
						b0_6						:1,		//
		    			b0_7						:1;		//
	}
	bit;
} param_flags_1_t;

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
				uint8_t	b0_0			:1,		//
		 				b0_1			:1,		//
		 				b0_2			:1,		//
						b0_3			:1,		//
						b0_4			:1,		//
						b0_5			:1,		//
						b0_6			:1,		//
		    			b0_7			:1;		//
	}
	bit;
} param_flags_2_t;

typedef struct __attribute__((packed))
{
	uint16_t				key;
	cfg_uart_t			cfg_uart[NUM_PORTE_COM];
	param_lan_com_t		param_lan_com[NUM_PORTE_LAN_COM];

	param_flags_1_t		param_flags_1;
	param_flags_2_t		param_flags_2;

	uint16_t				timeout_refresh_wd;					// Timeout oltre il quale scatta il PingWatchdog (in S)

	uint8_t				libero[14];
} masterParametri_t;

/*****************************************************************
	Buf ping
******************************************************************/
typedef union __attribute__((packed))
{
	uint8_t	byte[2];
	struct __attribute__((packed))
	{
				uint8_t	rl_scambio_lan_ts		:1,		//
						rl_scambio_lan_voip		:1,		//
		 				rl_scambio_bf_voip		:1,		//
		 				rl_scambio_n_r			:1,		//
		 				cmd_dsp_on				:1,		//
						cmd_pwd_543				:1,		//
						cmd_rst_up_543			:1,		//
		    			cmd_mst_reset_bkf		:1;		//

				uint8_t	cmd_en_alim_bkf			:1,		//
		 				out_rear_2				:1,		//
						out_rear_1				:1,		//
						b1_3					:1,		//
						b1_4					:1,		//
						b1_5					:1,		//
						b1_6					:1,		//
		    			b1_7					:1;		//
	}
	bit;
} stato_pin_out_t;

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
				uint8_t	in_rear_1				:1,		//
		 				in_rear_2				:1,		//
		 				b0_2					:1,		//
						b0_3					:1,		//
						b0_4					:1,		//
						b0_5					:1,		//
						b0_6					:1,		//
		    			b0_7					:1;		//
	}
	bit;
} stato_pin_in_t;

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
				uint8_t	disable_ping_rx			:1,		// Bit utilizzato in fase di mantenzione per bloccare i comandi del buffer di ping
																				// Viene automaticamente disattivato a tempo dopo x minuti di mancato refresh del bit	 		
		 		lan_manuale								:1,		// Stato di lan controllata manualmente 	 		
		 		b0_2											:1,		//
				b0_3											:1,		//
				b0_4											:1,		//
				b0_5											:1,		//
				b0_6											:1,		//
		    b0_7											:1;		//
	}
	bit;
} flags_ping_tx_t;

typedef struct __attribute__((packed))
{
	uint16_t				check_word;					// 0x00 - 0x01	Valore di controllo di validità area (0x55AA)
	version_t			MASTER_ver;					// 0x02 - 0x07
			
	uint16_t				timeout_refresh_wd;			// 0x08 - 0x09	Timeout oltre il quale scatta il PingWatchdog (in S)
	uint8_t				reset_counter;				// 0x0A			Copia del valore di reset counter di over_mem
	uint8_t				reset_wd_sw;				// 0x0B			Contatore del numero di auto-reset generati dal wd_sw
	uint8_t				num_switch_rete;			// 0x0C			Contatore del numero di switch effettuati dall'automa "verifica_connessione"
	word_bits_t			stato_lan_com;				// 0x0D	- 0x0E	ogni bit corrisponde allo stato fisico delle porte lan-com (1 = porta connessa)

	stato_pin_out_t		stato_pin_out;			// 0x0F	- 0x10
	stato_pin_in_t		stato_pin_in;			// 0x11

	flags_ping_tx_t		flags;					// 0x12			Byte di flags varie
	uint32_t				uptime;					// 0x13 - 0x16	Uptime
	
	uint8_t				liberi[0x0A];
} buf_ping_tx_0_t;	// Dimensione totale 32 byte

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
				uint8_t	agg_refresh_wd_ram			:1,		//
		 				agg_refresh_wd_ram_flash	:1,		//
		 				reset_ts					:1,		// Richiesta di reset del TS ma ritardata
						azzera_num_reset			:1,		//
						lan_manuale					:1,		// Quando questo bit è 1, blocco automa di verifica connessione ed abilito scrittura porte rl_scambio_lan_ts e rl_scambio_lan_voip
						b0_5						:1,		//
						b0_6						:1,		//
		    			b0_7						:1;		//
	}							
	bit;
} cmd_ric_t;

typedef struct __attribute__((packed))
{
	stato_pin_out_t		cmd_pin_out;
	cmd_ric_t			cmd_ric;
	uint16_t				timeout_refresh_wd;		// 0x08 - 0x09	Timeout oltre il quale scatta il PingWatchdog (in S)
														// 				NOTA: il timeout verrà aggiornato solo se agg_refresh_wd_ram o agg_refresh_wd_ram_flash?
} buf_ping_rx_0_t;


/*****************************************************************
	OROLOGI
******************************************************************/
typedef struct
{
	uint16_t	orol_interrupt_Timer;	//timer prescaler Timer2 500uSec
	uint16_t	t_lampeggio;			//timer per lampeggio sincronizzato
	uint16_t	t1mS;
	uint16_t	t5mS;
	uint16_t	t10mS;
	uint16_t	t100mS;
	uint16_t	t1S;
}MasterTimer_ticks_t;

typedef struct
{
	uint16_t PingTimer_1ms;
}MasterTimer_u16_1mS_t;


typedef struct
{
	uint16_t	libero;
}MasterTimer_u16_10mS_t;


typedef struct
{
	uint16_t	init;
	uint16_t	adc;
	uint16_t	readInput[IO_PORTS];	// per filtro ingressi digitali
	uint16_t	port_tout[NUM_GES_COM];
	uint16_t	rst_spi_uart;
	uint16_t	spi_uart[NUM_PORTE_COM];
	uint16_t	LanCom[NUM_PORTE_LAN_COM];
	uint16_t	front_led;
	uint16_t	PingTimer;
	uint16_t	start_dsp;
	uint16_t	start_voip;
}MasterTimer_u16_5mS_t;

typedef struct
{
	uint16_t	ges_msg_tout[NUM_GES_COM];				//orologi per timeout gestione di un messaggio (uno per ogni gestore comandi)
	uint16_t	startup;
	uint16_t	PingTimer_100ms;
	uint16_t	verifica_connessione;
	uint16_t	lan_stat;
	uint16_t	GesConfLan;
	uint16_t	ritardo_reset;
	uint16_t	ritardo_en_bkf;
	uint16_t	LanComListenRestart[NUM_PORTE_LAN_COM];
	uint16_t	GesComListenRestart[NUM_GES_COM_LAN];
}MasterTimer_u16_100mS_t;

typedef struct
{
	uint16_t	start_wd_sw;
	uint16_t	refresh_wd_sw;
	
	uint16_t	timeout_disable_ping_rx;
	uint16_t	lanComWrTimeout[NUM_PORTE_LAN_COM];
	
}MasterTimer_u16_1S_t;

typedef struct
{
	uint32_t uptime;
}MasterTimer_u32_1S_t;

typedef union __attribute__((packed))
{
	uint16_t	word;
	struct __attribute__((packed))
	{
				uint16_t	buffer_ready			:1,		//Flag di buffer pronto
							char_tx_ok				:1,		//Ok carattere TX
		 					length					:14;	//Lunghezza buffer
	}
	bit;
} buf_flags_t;


#endif	/* MASTER_TYPE_H */

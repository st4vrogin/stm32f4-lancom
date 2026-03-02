/*
 * CommonDef.h
 *
 *  Created on: Feb 14, 2026
 *      Author: Siel Tre
 */

#ifndef COMMON_INC_COMMONDEF_H_
#define COMMON_INC_COMMONDEF_H_
#include "Protocol/SielDef.h"
#include "usbd_desc.h"

/*****************************************************************
	Registri UID cortex
******************************************************************/

#define FLASH_SIZE_BASE		((uint32_t)0x1FFF7A22)		// For low and medium size micro


/*****************************************************************
	Struttura Versione Sw
	Non deve essere allocata
	Dovrà esserci solo un puntatore a questa struttura
******************************************************************/
typedef struct __attribute__((packed))
{
	uint8_t	ver[3];
}versione_sw;

/*****************************************************************
	Struttura Data di Rilascio Sw
	Non deve essere allocata
	Dovrà esserci solo un puntatore a questa struttura
******************************************************************/
typedef struct __attribute__((packed))
{
	uint8_t	r_year;
	uint8_t	r_month;
	uint8_t	r_day;
}rilascio_sw;

/*****************************************************************
	Struttura Dati Informativi FW
******************************************************************/
typedef struct __attribute__((packed))
{
	versione_sw	version;
	rilascio_sw	rel_date;
}version_t;

typedef union __attribute__((packed))
{
   	uint8_t	byte;
   	struct __attribute__((packed))
	{
		uint8_t	b0	:1,
				b1	:1,
				b2	:1,
				b3	:1,
				b4	:1,
				b5	:1,
				b6	:1,
				b7	:1;
	}bit;
}bits_t;

typedef union __attribute__((packed))
{
   	uint16_t	word;
   	struct __attribute__((packed))
	{
		uint16_t	b0	:1,
					b1	:1,
					b2	:1,
					b3	:1,
					b4	:1,
					b5	:1,
					b6	:1,
					b7	:1,

					b8	:1,
					b9	:1,
					b10	:1,
					b11	:1,
					b12	:1,
					b13	:1,
					b14	:1,
					b15	:1;
	}bit;
}word_bits_t;

/*****************************************************************
	Define generali di sistema
******************************************************************/
#define		VALID_W32		0x55AA55AA
#define		VALID_W16		0x55AA


/*****************************************************************
	Flag generali di sistema
******************************************************************/

typedef union __attribute__((packed))
{
    uint8_t		byte[2];
    struct __attribute__((packed))
	{
		uint8_t	stay_loader			:1,		//rimane in loader
				DisableFlash		:1,		//
				b0_2				:1,		//
				req_jmp_loader_rit	:1,		// jmp in kernel con ritardo
				req_jmp_main_rit	:1,		// jmp in main con ritardo
				req_reset			:1,		// 1= riavvio Kernel
				b0_6				:1,		//
				runMain				:1;		// 1= sistema in Main

		uint8_t	lampeggio 			:1,		//flag lampeggio comune a tutti i led (in appl)
				b1_1				:1,		//
				tick_5mS			:1,		//tic di sistema da 5 ms
				b1_3				:1,		//
				tick_1mS			:1,		//tic di sistema da 1 ms
				rele_ts_pos			:1,		//Salvataggio posizione rele per avvio
				rst_mt				:1,		// Reset per mancanza tensione
				b1_7				:1;
	}bit;
}sys_flag_t;

/*****************************************************************
	Strutture dati helper CORTEX M4
******************************************************************/
typedef struct
{
	uint32_t	uid[3];
}UID_TypeDef;

typedef struct
{
	uint16_t	flash_size;
}FLASH_SIZE_TypeDef;

#define UID					((UID_TypeDef *)UID_BASE)
#define FLASH_SIZE			((FLASH_SIZE_TypeDef *)FLASH_SIZE_BASE)

/*****************************************************************
	Oggetti di sistema da mantenere anche dopo reset
******************************************************************/
typedef struct __attribute__((packed))
{
	uint16_t	check_word;					// Valore di controllo di validità area OverMemory (VALID_W32)
	uint8_t		reset_counter;				// Contatore di reset
	sys_flag_t	sys_flag;					// flag di sistema
	uint16_t	rcc_csr;
	uint8_t		AddressScheda[ADDR_LEN];	// Indirizzo di protocollo della scheda  3 byte
	uint16_t	timeout_ric_reset_5mS;		// Richiesta reset ritardato in step di 5mS
}over_mem_t;


/*****************************************************************
	PARAMETRI DI RETE
******************************************************************/
typedef struct __attribute__((packed))
{
	uint16_t	key;

	uint8_t		usa_dhcp;			// 0 disattivo, 1 attivo
	uint8_t		ip_addr[4];			// Indirizzo lan da usare se (usa_dhcp = 0) oppure se dhcp non funzionante.
								//		I dati sono da considerarsi in quest'ordine (es.):
								//		adr_lan[0] = 192;
								//		adr_lan[1] = 168;
								//		adr_lan[2] = 1;
								//		adr_lan[3] = 100;
	uint8_t		netmask_addr[4];	// Come sopra
	uint8_t		gw_addr[4];			// Come sopra

	uint16_t	porta_ges_com[NUM_GES_COM_LAN];

} commonParametri_lan_t;

typedef struct __attribute__((packed))
{
	uint16_t	key;				//0-1
	uint8_t		mac_scritto;		//2 Se = 0x00 -> non è ancora stato sostituito da quello di fabbrica
	uint8_t 	mac_address[6];		//3-8 MAC Address della scheda
	uint8_t		libero[32-9];		//9-31
} ethParametri_t;

#ifndef PARAM_SBANC_LEN
#define PARAM_SBANC_LEN			sizeof(ethParametri_t) + sizeof(commonParametri_lan_t) + sizeof(commonParametri_lan_t) //+ sizeof(masterParametri_t) + sizeof(masterParametri_t)
#endif

#define DYN_GES_STREAMBUF_SIZE	300 //Dimensione degli streambuffer dinamicamente allocati

typedef	uint16_t			(* retu16_func)(void);
typedef	uint16_t			(* retu16_funcu8)(uint8_t);
typedef void				(* void_funcu8)(uint8_t);
typedef void				(* void_func)(void);

/*****************************************************************
	Struttura orologi di sistema a 5 msec
******************************************************************/
typedef struct
{
	uint16_t	ritardo_reset;
	uint16_t	ritardo_jmp_main;
}systmr_b_5ms_t;


/*****************************************************************
	Definizioni per StatoLAN
******************************************************************/
typedef struct __attribute__((packed))
{
	uint8_t	first_time_configured;
	uint8_t	reconf_request;
	uint8_t	reconf_wait;
	uint8_t	lan_ready;
} stato_lan_fields_t;

typedef union __attribute__((packed))
{
	uint32_t				data;
	stato_lan_fields_t		fields;
} stato_lan_t;


#define		TASK_RUNNING			0
#define		TASK_SUSPENDED			1

#endif /* COMMON_INC_COMMONDEF_H_ */

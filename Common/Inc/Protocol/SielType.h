#ifndef COMMON_INC_PROTOCOL_SIELTYPE_H_
#define COMMON_INC_PROTOCOL_SIELTYPE_H_
#include <stdint.h>
#include "CommonDef.h"
#include "SielDef.h"


typedef struct __attribute__((packed))
{
	uint8_t 		from;		//Mittente interno
	uint8_t 		to;			//Destinatario interno
	uint16_t 		len;		//Lunghezza payload netto

} MessageHeader_t;


typedef union __attribute__((packed))
{
    uint8_t	byte;
    struct __attribute__((packed))
	{
    	uint8_t	no_chksum	:1,				//bit 0 il campo checksum non è calcolato (e quindi non deve essere verificato)
				libero1 	:1,
				tmoLong 	:1,				// il messaggio richede l'utilizzo del timeout "lungo"
				no_waitAns 	:1,				//non aspetta risposta perché il comando non la richiede
											//serve per i comandi passanti (il micro passante non deve attendere la risposta
											//dal micro destinatario né andare in timeout passante
				ack			:4;				//bit 4-5-6-7
	}	bit;
}flag_t;			//flag messaggio


typedef struct __attribute__((packed))
{
    MessageHeader_t header;
    uint8_t			gestore;
    uint8_t			mitt[ADDR_LEN];
    uint8_t			dest[ADDR_LEN];
    uint8_t			cod_op;
    flag_t 			flag;
    uint8_t 		*netto;
} SielMessage_t;


#define	MAX_COM_OVHD		(sizeof(SielMessage_t) - sizeof(MessageHeader_t) - sizeof(uint8_t *))	// header Campi fissi cmd  PTCL MITT DEST COP FLAG
#define	DIMBUF_GES_COM		(1 + MAX_COM_DATA_UART + MAX_COM_OVHD + 2)

typedef union __attribute__((packed))
{
	uint8_t	raw[DIMBUF_GES_COM];
	struct __attribute__((packed))
	{
		uint8_t	gestore;
		uint8_t	mitt[ADDR_LEN];
		uint8_t	dest[ADDR_LEN];
		uint8_t	cod_op;
		flag_t 	flag;
		uint8_t	dati[DIMBUF_GES_COM-MAX_COM_OVHD];
	}cmd;

	struct __attribute__((packed))
	{
		uint8_t	gestore;
		uint8_t	mitt[ADDR_LEN];
		uint8_t	dest[ADDR_LEN];
		uint8_t	cod_op;
		flag_t 	flag;
		uint8_t	dati[DIMBUF_GES_COM -MAX_COM_OVHD];
	}risp;

}comBuf_t;		//buffer comunicazione

typedef struct __attribute__((packed))
{
	uint8_t     tx_ok	:1,				//flag trasmissione completata
		      stato :7;				//stato comDati
}stCom_t;
typedef struct __attribute__((packed))
{
		stCom_t			st;
		uint16_t		len_buf;
		uint16_t		nchar_TX;
		comBuf_t		buf;
}comDati_t;			//dati porta comunicazione



typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
				uint8_t	overflow					:1,		// Segnalazione di overflow
		 				b0_1						:1,		//
						b0_2						:1,		//
						b0_3						:1,		//
						b0_4						:1,		//
						b0_5						:1,		//
						b0_6						:1,		//
		    			b0_7						:1;		//
	}
	bit;
} comm_flags_t;

typedef struct
{
	uint8_t 		data[MAX_SERIAL_PKT_SIZE];
	uint16_t 		len;
	comm_flags_t 	flags;
} SerialPacket_t;

typedef enum { COUNT_STUFF, COUNT_DESTUFF } opCount_e;

typedef enum
{
	SPS_WAIT_STX,
	SPS_WAIT_ETX,
	SPS_SPECIAL_BYTE,
	SPS_END
} e_InPlaceSielProtocolStatus;

typedef struct
{
	retu16_func	func;
} cod_op_func_t;


#endif

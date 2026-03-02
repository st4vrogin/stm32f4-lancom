#ifndef COMMON_INC_PROTOCOL_SIELDEF_H_
#define COMMON_INC_PROTOCOL_SIELDEF_H_

#define ADDR_LEN			(unsigned int)3
#define	MAX_COM_DATA_UART	300		// Quantità massima di dati gestita da UART

#define NUM_GES_COM			5
#define NUM_GES_COM_LAN		3

#define	GES_COM_CODE			0x20		// Codice Gestore Comandi
#define	PTCL_BYTE_START			0x02		// STX
#define	PTCL_BYTE_STOP			0x03		// ETX
#define	PTCL_BYTE_STUFF			0x10		// Codice Byte Stuffing
#define	PTCL_BYTE_DESTUFF		0x80		// Offset Byte Stuffing

#define ERR_OK                   0
#define ERR_MEM					100
#define ERR_BUF_TOO_SMALL		-1
#define ERR_INVALID_FORMAT		-2
#define ERR_INVALID_BUFFER		-3

#define MAX_SERIAL_PKT_SIZE		312

#define ENTITY_CPU				0
#define ENTITY_GESCOM			1
#define ENTITY_UART				2
#define ENTITY_USB				4
#define ENTITY_LAN_GES_0		10
#define ENTITY_LAN_GES_1		11
#define ENTITY_LAN_GES_2		12
#define ENTITY_BROADCAST		0xFF

	//per i Codici di Ritorno si utilizzano solo i 3 bit più significativi
#define	PTCL_ACK				0			// 0b000 - ACK
#define	PTCL_ERR_TELEGRAM		1			// 0b001 - Errore Checksum
#define	PTCL_ERR_COP_NOT_SUP	2			// 0b010 - CodOp non presente
#define	PTCL_ERR_DATI			3			// 0b011 - Errore Dati
#define PTCL_ERR_FLASH_DISABLE	4			// 0b101 - Comando vietato perché la Flash è disabilitata
#define PTCL_ERR_PORTA_BUSY		5			// 0b101 - Porta passante occupata
#define PTCL_ERR_FWD_TOUT		6			// 0b110 - Timout passante generato da Ges_com che invia su UART
#define	PTCL_ERR_GEN			7			// 0b111 - Errore non specificato

// Indirizzo broadcast di scheda Base Master
#define BROADCAST_0				0x5C
#define BROADCAST_1				0x80
#define BROADCAST_2				0x00

#define		USB_XCHNG_BUF_SIZE			380
#define 	USB_QUEUE_LENGTH  			8
#define 	LAN_GESCOM_QUEUE_LENGTH  	8

#endif

/*
 * Codop.h
 *
 *  Created on: Feb 14, 2026
 *      Author: Siel Tre
 */

#ifndef COMMON_INC_PROTOCOL_CODOP_H_
#define COMMON_INC_PROTOCOL_CODOP_H_
#include "Protocol/SielType.h"
#include "CommonDef.h"


/*****************************************************************
	cod_op_02:
		parametri:
			uint8	subcode			0 -> dati del processore
									1 -> versione kernel
		dati restituiti:
			vedi strutture dati
******************************************************************/
typedef struct __attribute__((packed))
{
	uint32_t		device_id;
	uint32_t		uid[3];
	uint16_t		flash_size;
}p_cop_02_00_t;

/****************************************************************
	p_cop_04_06_0C_0E_t: lettura / scrittura / OR / AND memoria su 4 byte
		parametri / dati restituiti:
			uint32	addr
			uint16	len
			uint8	dati / uint8	dati[len]
****************************************************************/
typedef struct
{
	uint32_t		addr;
	uint16_t		len;
}p_cop_04_06_0C_0E_t;

/****************************************************************
	p_cop_08_t: comandi parametrici
		parametri
			uint8	param
			uint8	dati[3]
		dati restituiti:
			null
****************************************************************/
typedef struct
{
	uint8_t	param;
	uint8_t	dati[3];
}p_cop_08_t;

/****************************************************************
	p_cop_10_t: Cancellazione flash
		parametri / dati restituiti:
			uint32	addr_in
			uint32	len
****************************************************************/
typedef struct
{
	uint32_t addr;
	uint32_t len;
}p_cop_10_t;

/*****************************************************************
	cod_op_12: Gestione CC2500
		parametri:
			uint8	subcode_0
			uint8	subcode_1
		dati restituiti:
			uint8	subcode_0
			uint8	subcode_1
			uint8	dati
******************************************************************/
typedef struct
{
	uint8_t	subcode_0;
	uint8_t	subcode_1;
	uint8_t	dati;
}p_cop_12_t;

/****************************************************************
	p_cop_14_t: Calcolo checksum
		parametri
			uint8	param;
			uint32	addr_start
			uint32	addr_end
		dati restituiti:
			uint8	param;
			uint8	stato;
			uint32	checksum;

****************************************************************/
//typedef struct
typedef struct __attribute__((packed))
{
	uint8_t		param;
	uint32_t	addr_start;
	uint32_t	addr_end;
}p_cop_14_rx_t;

//typedef struct
typedef struct __attribute__((packed))
{
	uint8_t		param;
	uint8_t		stato;
	uint32_t	checksum;
}p_cop_14_tx_t;

/****************************************************************
	p_cop_16: Gestione LAN
****************************************************************/
typedef struct __attribute__((packed))
{
	uint8_t			param;
	ethParametri_t	fabParam;
}p_cop_16_0_1_rtx_t;

typedef struct __attribute__((packed))
{
	uint8_t					param;
	commonParametri_lan_t	lan_conf;
}p_cop_16_2_7_rtx_t;

uint16_t	GesCodOp(uint8_t codop);
uint16_t	cod_op_02(void);
uint16_t	cod_op_04(void);
uint16_t	cod_op_06(void);
uint16_t	cod_op_08(void);
uint16_t	cod_op_0A(void);
uint16_t	cod_op_0C(void);
uint16_t	cod_op_0E(void);
uint16_t	cod_op_10(void);
uint16_t	cod_op_14(void);
uint16_t	cod_op_16(void);
uint16_t	cod_op_mancante_k(void);

#endif /* COMMON_INC_PROTOCOL_CODOP_H_ */

/*****************************************************************

	Project:

	Module:		ApplcodOp.h
	Created:
	Descr:		gestione codici operativi di Applicativo

	Last Rev.:

******************************************************************/

#ifndef __APPL_COD_OP_H__
#define __APPL_COD_OP_H__
#include "appl_types.h"


/****************************************************************
	p_cop_22: Gestione PARAMETRI
****************************************************************/
typedef struct __attribute__((packed))
{
	uint8_t					param;
	union __attribute__((packed))
	{
		masterParametri_t		masterParametri;
		char					password[PROTECTED_COP_PWD_LEN];
	} data;
}p_cop_22_rtx_t;

typedef struct __attribute__((packed))
{
	buf_ping_tx_0_t		buf_ping_tx_0;
}p_cop_28_0_tx_0_t;

typedef struct __attribute__((packed))
{
	buf_ping_rx_0_t		buf_ping_rx_0;
}p_cop_28_10_rx_0_t;


typedef struct __attribute__((packed))
{
	uint8_t				disable_ping_rx;
	uint16_t			timeout_disable_ping_rx;
}p_cop_28_20_rx_0_t;

uint16_t	ApplCodOp(uint8_t CodOp);

#endif	/*  __APPL_COD_OP_H__ */

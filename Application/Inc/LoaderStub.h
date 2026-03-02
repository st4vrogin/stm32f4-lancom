/** ****************************************************************************
	@file		LoaderStub.h
	@brief		Definizione tipi e costanti per compatibilità con BootLoader

	@author		Siel Tre srl
	@version	1.0.0	2023-07-21
******************************************************************************/
#ifndef _LOADER_STUB_H_
#define _LOADER_STUB_H_
#include "CommonDef.h"

// definizioni tipi di record info
#define INFO_CODE_CHECK			1		// verifica crc o checksum
#define INFO_VER_LDR			2		// versione programma Loader
#define INFO_VER_APP			3		// versione Applicazione

// tipi di verifica di congruenza previsti
#define CHECK_TYPE_CKS32		1		// checksum-32
#define CHECK_TYPE_CRC16		2		// CRC 32-bit TODO: correggere define nella prossima versione di loader
#define CHECK_TYPE_KEY64		3		// chiave fissa a 64 bit

/*****************************************************************
	Struttura Dati Informativi FW di applicazione
******************************************************************/
typedef struct __attribute__((packed))
{
	versione_sw	version;			//0-2
	rilascio_sw	rel_date;			//3-5
	uint8_t		app;				//6
	uint8_t		model;				//7
} version_app_t;

// struttura intestazione record
typedef struct __attribute__((packed))
{
	uint8_t RecordType_1;					// 0
	uint8_t RecordSize_1;					// 1
	uint8_t RecordType_2;					// 2
	uint8_t RecordSize_2;					// 3
} info_rec_head_t;

// struttura per record INFO_CODE_CHECK - verifica congruenza codice
typedef struct __attribute__((packed))
{
	info_rec_head_t RecHeader;			// 0-3
	uint8_t CheckType;					// 4
	uint32_t *StartAddress;				// 5-8
	uint32_t *EndAddress;					// 9-12
	uint8_t Free[3];						// 13-15
	uint64_t CheckValue;					// 16-23
} info_code_check_t;

// struttura per record INFO_VER_LDR - dati di versione del Loader
typedef struct __attribute__((packed))
{
	info_rec_head_t RecHeader;			// 0-3
	version_app_t Version;				// 4-11
	uint8_t BuildYear;					// 12
	uint8_t BuildMonth;					// 13
	uint8_t BuildDay;						// 14
	uint8_t BuildHour;					// 15
	uint8_t BuildMin;						// 16
	uint8_t BuildSec;						// 17
	uint8_t Free[6];						// 18-23
} info_ver_ldr_t;



#endif //LOADER_STUB

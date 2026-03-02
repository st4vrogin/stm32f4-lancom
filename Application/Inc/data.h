/*
 * data.h
 *
 *  Created on: Feb 11, 2026
 *      Author: Siel Tre
 */

#ifndef APPLICATION_INC_DATA_H_
#define APPLICATION_INC_DATA_H_
#include "appl_types.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "ApplBuild.h"
#include "spi_uart.h"
#include "LoaderStub.h"
#include "event_groups.h"
#include "circ_buffer.h"
#include "FreeRTOS_sockets.h"

#ifndef COMPILE_DATA
#define EXT 		extern
#define VOL 		extern volatile
#define BSRAM_I 	EXT
#define SER_USR 	EXT const
#define LDR_INF		EXT const
#else
#define EXT
#define VOL				volatile
#define BSRAM_I 		__attribute__((section(".bsram_info"))) volatile
#define SER_USR			const __attribute__((section(".param_seriale_flash_usr")))
#define LDR_INF			const __attribute__((section(".appSwDesc")))
#endif

EXT const version_t MASTER_ver
#ifdef COMPILE_DATA
= {
		.version.ver = { (uint8_t)APPL_VER_0, (uint8_t)APPL_VER_1, (uint8_t)APPL_VER_2 },
		.rel_date = { .r_year = (uint8_t)APPL_BYEA, .r_month = (uint8_t)APPL_BMON, .r_day = (uint8_t)APPL_BDAY }
}
#endif
;

EXT outDig_L_t					outputDigitali[2];
EXT stato_IO_t					stato_IO;
BSRAM_I	buf_ping_tx_0_t			buf_ping_tx_0;
EXT buf_ping_rx_0_t				buf_ping_rx_0;
EXT word_bits_t					phyOut[IO_PORTS];
EXT	word_bits_t 				precIn[IO_PORTS];
EXT BaseType_t					manOutput;
EXT MasterFlag_t				MasterFlag;
EXT QueueHandle_t 				usbRxQueue;

EXT masterParametri_t			masterParametri;
EXT sistema_t					sistema;
EXT uint32_t					debug[16];
EXT masterCmd_t					masterCmd;

EXT MasterTimer_ticks_t			MasterTimer_ticks;
EXT MasterTimer_u16_1mS_t		MasterTimer_u16_1mS;
EXT MasterTimer_u16_5mS_t		MasterTimer_u16_5mS;
EXT MasterTimer_u16_10mS_t		MasterTimer_u16_10mS;
EXT MasterTimer_u16_100mS_t		MasterTimer_u16_100mS;
EXT MasterTimer_u16_1S_t		MasterTimer_u16_1S;
EXT MasterTimer_u32_1S_t		MasterTimer_u32_1S;

EXT const masterParametri_t		masterParametri_flash_fab
#ifdef COMPILE_DATA
#include "Defaults/PrmSerialiFab.h"
#endif
;

SER_USR masterParametri_t		masterParametri_flash;

EXT	registri_spi_uart_t			registri_spi_uart[NUM_PORTE_COM];
EXT	code_t						p_coda_rx_spi[NUM_PORTE_LAN_COM];
EXT EventGroupHandle_t lan2ComEvGroupHandle;
EXT EventGroupHandle_t com2LanEvGroupHandle;

LDR_INF info_code_check_t keySWDesc
#ifdef COMPILE_DATA
= {
	{
		INFO_CODE_CHECK,
		24,
		INFO_CODE_CHECK,
		24
	},
	CHECK_TYPE_KEY64,
	(uint32_t *)0x08120000,
	(uint32_t *)0x0813FFFF,
	{0,0,0},
	0x55AA55AA55AA55AA
}
#endif //MASTER_DATI_C
;

VOL CircularBuffer_t bufferSpiRx[NUM_PORTE_LAN_COM];
VOL CircularBuffer_t bufferSpiTx[NUM_PORTE_LAN_COM];

EXT Socket_t		LancomClients[10]
#ifdef MASTER_DATI_C
= { NULL }
#endif
;

EXT	uint16_t		timeout_disable_ping_rx;


#endif /* APPLICATION_INC_DATA_H_ */

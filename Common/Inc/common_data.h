/*
 * common_data.h
 *
 *  Created on: Feb 14, 2026
 *      Author: Siel Tre
 */

#ifndef COMMON_INC_COMMON_DATA_H_
#define COMMON_INC_COMMON_DATA_H_
#include "FreeRTOS.h"
#include "queue.h"
#include "stream_buffer.h"
#include "Protocol/SielDef.h"
#include "CommonDef.h"
#include "Protocol/SielType.h"

#ifndef COMPILE_COMMON_DATA
#define EXT extern
//#define BSRAM_I extern
#define NOINIT		EXT
#define ETH_PARAM	EXT const
#define PRM_LAN1	EXT const
#define PRM_LAN2	EXT const
#else
#define EXT
//#define BSRAM_I 		__attribute__((section(".bsram_info"))) volatile
#define NOINIT 			__attribute__((section(".noinit"))) volatile
#define ETH_PARAM		const __attribute__((section(".param_ethernet")))
#define PRM_LAN1		const __attribute__((section(".param_lan_flash_1")))
#define PRM_LAN2		const __attribute__((section(".param_lan_flash_2")))
#endif

NOINIT	over_mem_t					over_mem;
EXT		StreamBufferHandle_t 		usbStreamBuffer;
EXT		StreamBufferHandle_t 		lgcStreamBufferDp[NUM_GES_COM_LAN];
EXT		uint8_t 					usbExchangeBuffer[USB_XCHNG_BUF_SIZE];
EXT 	QueueHandle_t				gescomRxQueue;
EXT		QueueHandle_t		 		lgcQueueTx[NUM_GES_COM_LAN];
EXT		comDati_t					*ptrBufCom;					//puntatore al buffer di cominicazione attivo (utilizzato da ApplCodOp)
EXT		comDati_t					ComDatiGescom;
EXT 	uint8_t						param_sbanc_ram[PARAM_SBANC_LEN];
EXT 	QueueHandle_t 				usbTxQueue;
EXT		retu16_funcu8 				pApplCodop;
EXT		systmr_b_5ms_t				systmr_b_5ms;
EXT		void_func					pApplServersStop;
EXT		void_func					pApplServersStart;

// Parametri di installazione
ETH_PARAM ethParametri_t	fabParametri
#ifdef COMPILE_COMMON_DATA
 = 	{
	#include "Defaults/ParamEth.h"
	}
#endif
;

EXT 		commonParametri_lan_t	commonParametri;
PRM_LAN1	commonParametri_lan_t	commonParametri_flash_1
#ifdef COMPILE_COMMON_DATA
= {
	#include "Defaults/ParamLan.h"
}
#endif
;
PRM_LAN2	commonParametri_lan_t	commonParametri_flash_2
#ifdef COMPILE_COMMON_DATA
= {
	#include "Defaults/ParamLan.h"
}
#endif
;

EXT stato_lan_t						stato_lan;

#endif /* COMMON_INC_COMMON_DATA_H_ */

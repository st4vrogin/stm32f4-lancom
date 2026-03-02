/******************************************************************************
	@prog		TERMINAL SERVER
	@file		ApplicationDef.h
	@brief		Define di applicazione

	@author		SielTre srl
	@version	1.0.0	2023-06-16
******************************************************************************/
#ifndef APPLICATION_DEFS_H_
#define APPLICATION_DEFS_H_
#include "macros.h"
#include "common_data.h"

/*****************************************************************
	DEFINIZIONI GENERICHE
*****************************************************************/

#define	NUM_PORTE_COM		10		// Questi 2 dati potrebbero non combaciare
#define	NUM_PORTE_LAN_COM	10		// per ora consideriamo che tutte le porte seriali sono lan_com

#define NUM_OROL_GEST_IO	9		//0: local_counter, 1-5: num_porte_input, 6: start_voip, 7: start_dsp


/*****************************************************************
	AUTOMI MASTER
*****************************************************************/
#define AUTO_SPI_UART			0	// 10 automi come segue
//#define AUTO_SPI_UART_0			0
//#define AUTO_SPI_UART_1			1
//#define AUTO_SPI_UART_2			2
//#define AUTO_SPI_UART_3			3
//#define AUTO_SPI_UART_4			4
//#define AUTO_SPI_UART_5			5
//#define AUTO_SPI_UART_6			6
//#define AUTO_SPI_UART_7			7
//#define AUTO_SPI_UART_8			8
//#define AUTO_SPI_UART_9			9
#define AUTOMA_VERIFICA_CONNESSIONE	11

#define MAX_AUTOMI_MASTER		24


#define LAMPEGGIO			over_mem.sys_flag.bit.lampeggio
/*****************************************************************
*	GESTIONE I/O
*****************************************************************/
#define	IO_PORTS		7		//n.porte I/O digitali
#define	NUM_ADC			3

#define SPI_CMD_UART_RST_BB				Perif_OutBit_BB((uint32_t)&(GPIOB->ODR), 6)
#define OUT_FRONT_1_BB					Perif_OutBit_BB((uint32_t)&(GPIOC->ODR), 7)
#define	OUT_BKF_SCAMBIO_NR_BB			Perif_OutBit_BB((uint32_t)&(GPIOC->ODR), 14)
#define LED_PCS_BB						Perif_OutBit_BB((uint32_t)&(GPIOE->ODR), 14)

#define AUTOM		0
#define MANUA		1

#define PA_IN	0
#define PB_IN	1
#define PC_IN	2
#define PD_IN	3
#define PE_IN	4
#define PF_IN	5
#define PG_IN	6

#define PA_OUT	0
#define PB_OUT	1
#define PC_OUT	2
#define PD_OUT	3
#define PE_OUT	4
#define PF_OUT	5
#define PG_OUT	6

#define MASK_INPUT_PA					0x0000
#define MASK_INV_IN_PA					0x0000
#define MASK_OUTPUT_PA					0x8000
#define MASK_INV_OUT_PA					0x0000

#define MASK_INPUT_PB					0x0000
#define MASK_INV_IN_PB					0x0000
#define MASK_OUTPUT_PB					0x0004
#define MASK_INV_OUT_PB					0x0000

#define MASK_INPUT_PC					0x0000
#define MASK_INV_IN_PC					0x0000
#define MASK_OUTPUT_PC					0x0080
#define MASK_INV_OUT_PC					0x0000

#define MASK_INPUT_PD					0x0000
#define MASK_INV_IN_PD					0x0000
#define MASK_OUTPUT_PD					0x0004
#define MASK_INV_OUT_PD					0x0000

#define MASK_INPUT_PE					0x3000
#define MASK_INV_IN_PE					0x0000
#define MASK_OUTPUT_PE					0x4080
#define MASK_INV_OUT_PE					0x4000

#define	OUT_RL_SCAMBIO_LAN_TS			outputDigitali[AUTOM].porta[PA_OUT].bit.b15
#define	OUT_RL_SCAMBIO_BF_VOIP			outputDigitali[AUTOM].porta[PB_OUT].bit.b2
#define	CMD_DSP_ON						outputDigitali[AUTOM].porta[PC_OUT].bit.b0
#define	OUT_RL_SCAMBIO_LAN_VOIP			outputDigitali[AUTOM].porta[PD_OUT].bit.b2
#define	OUT_CMD_PWD_543					outputDigitali[AUTOM].porta[PE_OUT].bit.b3
#define	OUT_CMD_RST_uP_543				outputDigitali[AUTOM].porta[PE_OUT].bit.b4
#define	OUT_CMD_MST_RESET_BKF			outputDigitali[AUTOM].porta[PE_OUT].bit.b5
#define	OUT_CMD_EN_ALIM_BKF				outputDigitali[AUTOM].porta[PE_OUT].bit.b6
#define	OUT_RL_SCAMBIO_N_R				outputDigitali[AUTOM].porta[PE_OUT].bit.b7
#define	OUT_REAR_2						outputDigitali[AUTOM].porta[PE_OUT].bit.b8
#define	OUT_REAR_1						outputDigitali[AUTOM].porta[PE_OUT].bit.b11

#define	IN_REAR_1						stato_IO.in.porta[PE_OUT].bit.b13
#define	IN_REAR_2						stato_IO.in.porta[PE_OUT].bit.b12


/*****************************************************************
*	VERIFICA CONNESSIONE
*****************************************************************/
#define	TIMEOUT_START_WD_SW		(5*60)		// Valori in secondi
#define	TIMEOUT_WD_SW_NORMAL	(2*60)
#define	TIMEOUT_DISABLE_PING_RX_1S	(60 * 60)

#define PROTECTED_COP_PWD_LEN	sizeof(uint32_t)

#define LISTEN_RESTART_TIMEOUT	10 //*100 ms dopo quanto tempo dopo la chiusura di una connessione servita devo far ripartire il socket in listen


/*****************************************************************
*	Sequenza di accensione VOIP e DSP (orologi a 10 mS)
*****************************************************************/
#define BOOT_VOIP_TIMEOUT		70
#define BOOT_DSP_TIMEOUT		20
#define BOOT_CLOCKS_SATURATION	3000

#define DIMBUF_LAN_COM			256

#ifdef PARAM_SBANC_LEN
#undef PARAM_SBANC_LEN
#endif
#define PARAM_SBANC_LEN			sizeof(ethParametri_t) + sizeof(commonParametri_lan_t) + sizeof(commonParametri_lan_t) + sizeof(masterParametri_t) + sizeof(masterParametri_t)

#endif

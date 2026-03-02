/*
 * gestio.c
 *
 *  Created on: Feb 11, 2026
 *      Author: Siel Tre
 */
#include <appl_tasks.h>
#include "common_data.h"
#include "data.h"
#include "defs.h"
#include "stm32f4xx_hal.h"

/********************************************************************
 * Private module declarations
 */
void 	rtosTaskGestIo(void *pvParameters);
void    ReadInput(void);
void    SetOutput(void);
void    WriteOutput(void);
void	filt_input(uint8_t num_porta, uint16_t porta_read);
void	initTaskGestIO(void);
//*******************************************************************

/********************************************************************
 * Private definitions
 */
#define IDX_LOCAL_COUNTER		0
#define IDX_VOIP				6
#define IDX_DSP					7
#define IDX_LED_PIASTRA			8
#define GESTIO_OROL_VOIP		MasterTimer_u16_5mS.start_voip
#define GESTIO_OROL_DSP			MasterTimer_u16_5mS.start_dsp
#define GESTIO_OROL_INPUT(x)	MasterTimer_u16_5mS.readInput[(x)]
#define THIS_STACK_SIZE			DEFAULT_STACK_SIZE
//*******************************************************************

/********************************************************************
 * Module variables
 */
static int8_t gestio_inited = 0;
static BaseType_t outputOK = pdFALSE;
static StaticTask_t thread_GestIo_tcb;	//Thread Control Block
static uint8_t gestio_stack[THIS_STACK_SIZE];
//*******************************************************************

const osThreadAttr_t gestIoTask_attributes = {
  .name = "GestIO",
  .stack_mem = gestio_stack,
  .stack_size = THIS_STACK_SIZE,
  .priority = (osPriority_t) osPriorityHigh,
  .cb_mem = &thread_GestIo_tcb,
  .cb_size = sizeof(thread_GestIo_tcb)
};


/**
 * @brief Routine di creazione del thread FreeRTOS
 *
 * Va richiamata in main e riportata in tasks.h
 */
osThreadId_t createTask_GestIO(void* arg)
{
	return osThreadNew(rtosTaskGestIo, arg, &gestIoTask_attributes);
}


/**
 * @brief Routine principale di gestione degli I/O.
 *
 * Si occupa della lettura dei contatti Input puliti e della scrittura sugli output.
 */
void rtosTaskGestIo(void *pvParameters)
{
	while(1)
	{
		if(gestio_inited == 0)
		{
			initTaskGestIO();
		}
		ReadInput();
		SetOutput();
		WriteOutput();
		// Gestione LED frontale
		if (OUT_RL_SCAMBIO_N_R == 0)
		{
			if (MasterTimer_u16_5mS.front_led < 20)
			{
				// LED acceso
				OUT_FRONT_1_BB = 1;
			}
			else if (MasterTimer_u16_5mS.front_led < 400)
			{
				// LED spento
				OUT_FRONT_1_BB = 0;
			}
			else
			{
				MasterTimer_u16_5mS.front_led = 0;
			}
		}
		else
		{
			OUT_FRONT_1_BB = 1;
		}
		osDelay(pdMS_TO_TICKS(GEST_IO_DEFAULT_WAIT_MS));
		//Le funzioni che usano gli oroglogi devono avere cura di resettarli o saturarli
	}
}

/**
 * @brief Routine di lettura degli input
 *
 * Si occupa della lettura dei contatti Input puliti.
 */
void    ReadInput(void)
{
	word_bits_t 	currIn[IO_PORTS];		//buffer temporaneo per lettura porte input

	/*
	if ( MasterFlag.bit.simInput == 1) return;

	if (MasterFlag.bit.tick_interrupt_rdin == 0) return;  // non è ora di eseguire
	MasterFlag.bit.tick_interrupt_rdin = 0;
	*/

	currIn[PA_IN].word = MASK_INV_IN_PA ^ (GPIOA->IDR & MASK_INPUT_PA);
	filt_input(PA_IN, currIn[PA_IN].word);

	currIn[PB_IN].word = MASK_INV_IN_PB ^ (GPIOB->IDR & MASK_INPUT_PB);
	filt_input(PB_IN, currIn[PB_IN].word);

	currIn[PC_IN].word = MASK_INV_IN_PC ^ (GPIOC->IDR & MASK_INPUT_PC);
	filt_input(PC_IN, currIn[PC_IN].word);

	currIn[PD_IN].word = MASK_INV_IN_PD ^ (GPIOD->IDR & MASK_INPUT_PD);
	filt_input(PD_IN, currIn[PD_IN].word);

	currIn[PE_IN].word = MASK_INV_IN_PE ^ (GPIOE->IDR & MASK_INPUT_PE);
	filt_input(PE_IN, currIn[PE_IN].word);

	// Aggiorno i valori degli I/O letti dal buffer di ping

	buf_ping_tx_0.stato_pin_out.bit.rl_scambio_lan_ts 	= OUT_RL_SCAMBIO_LAN_TS;
	buf_ping_tx_0.stato_pin_out.bit.rl_scambio_lan_voip = OUT_RL_SCAMBIO_LAN_VOIP;
	buf_ping_tx_0.stato_pin_out.bit.rl_scambio_bf_voip 	= OUT_RL_SCAMBIO_BF_VOIP;
	buf_ping_tx_0.stato_pin_out.bit.rl_scambio_n_r		= OUT_RL_SCAMBIO_N_R;
	buf_ping_tx_0.stato_pin_out.bit.cmd_dsp_on			= CMD_DSP_ON;
	buf_ping_tx_0.stato_pin_out.bit.cmd_pwd_543			= OUT_CMD_PWD_543;
	buf_ping_tx_0.stato_pin_out.bit.cmd_rst_up_543		= OUT_CMD_RST_uP_543;
	buf_ping_tx_0.stato_pin_out.bit.cmd_mst_reset_bkf	= OUT_CMD_MST_RESET_BKF;

	buf_ping_tx_0.stato_pin_out.bit.cmd_en_alim_bkf		= OUT_CMD_EN_ALIM_BKF;
	buf_ping_tx_0.stato_pin_out.bit.out_rear_2			= OUT_REAR_2;
	buf_ping_tx_0.stato_pin_out.bit.out_rear_1			= OUT_REAR_1;

	buf_ping_tx_0.stato_pin_in.bit.in_rear_1			= IN_REAR_1;
	buf_ping_tx_0.stato_pin_in.bit.in_rear_2			= IN_REAR_2;
}



/**
 * @brief Routine di impostazione dello stato di output
 *
 * Si occupa dell'impostazione degli output.
 * - Verifica la disponibilità del buffer intermedio per la scrittura
 * - Legge la flag AUTO o MAN
 * - Scrive nelle posizioni opportune dei buffer intermedi
 */
void	SetOutput(void)
{
	if (outputOK == pdTRUE) return;	        //buffer intermedio occupato (in attesa di essere attuato)

		//imposta origine (Auto o Manuale)
	stato_IO.ptOutDig = &(outputDigitali[MasterFlag.bit.manOutput]);		//imposta manuale o auto

	//Se sono in fase di startup forzo lo stato dei pin corripondenti al default
	//ALIM_BKF (PE6) --> DEFAULT 1 --> VOIP SPENTO
	if (GESTIO_OROL_VOIP < BOOT_VOIP_TIMEOUT)
		stato_IO.ptOutDig->porta[PE_OUT].bit.b6 = 1;
	else if (GESTIO_OROL_VOIP == BOOT_VOIP_TIMEOUT && stato_IO.ptOutDig->porta[PE_OUT].bit.b6 == 1)
		stato_IO.ptOutDig->porta[PE_OUT].bit.b6 = 0;
	//PWD_543 (PE3) --> DEFAULT 1 --> DSP SPENTO
	if (GESTIO_OROL_DSP < BOOT_DSP_TIMEOUT)
		stato_IO.ptOutDig->porta[PE_OUT].bit.b3 = 1;
	else if (GESTIO_OROL_DSP == BOOT_DSP_TIMEOUT && stato_IO.ptOutDig->porta[PE_OUT].bit.b3 == 1)
		stato_IO.ptOutDig->porta[PE_OUT].bit.b3 = 0;

	// Se sono in AUTOMATICO copio lo stato degli OUTPUT auto su OUTPUT man
	if (MasterFlag.bit.manOutput == 0)
	{
		outputDigitali[1].porta[PA_OUT].word = outputDigitali[0].porta[PA_OUT].word;
		outputDigitali[1].porta[PB_OUT].word = outputDigitali[0].porta[PB_OUT].word;
		outputDigitali[1].porta[PC_OUT].word = outputDigitali[0].porta[PC_OUT].word;
		outputDigitali[1].porta[PD_OUT].word = outputDigitali[0].porta[PD_OUT].word;
		outputDigitali[1].porta[PE_OUT].word = outputDigitali[0].porta[PE_OUT].word;
	}

	phyOut[PA_OUT].word = stato_IO.ptOutDig->porta[PA_OUT].word ^ MASK_INV_OUT_PA;
	phyOut[PB_OUT].word = stato_IO.ptOutDig->porta[PB_OUT].word ^ MASK_INV_OUT_PB;
	phyOut[PC_OUT].word = stato_IO.ptOutDig->porta[PC_OUT].word ^ MASK_INV_OUT_PC;
	phyOut[PD_OUT].word = stato_IO.ptOutDig->porta[PD_OUT].word ^ MASK_INV_OUT_PD;
	phyOut[PE_OUT].word = stato_IO.ptOutDig->porta[PE_OUT].word ^ MASK_INV_OUT_PE;

	//Saturazione orologi
	if(GESTIO_OROL_VOIP > BOOT_VOIP_TIMEOUT + 1)
		GESTIO_OROL_VOIP = BOOT_VOIP_TIMEOUT + 1;
	if(GESTIO_OROL_DSP > BOOT_DSP_TIMEOUT + 1)
		GESTIO_OROL_DSP = BOOT_DSP_TIMEOUT + 1;


	outputOK = pdTRUE;
}

/**
 * @brief Routine di scrittura degli output
 *
 * Si occupa della scrittura degli output sulle porte fisiche
 */
void	WriteOutput(void)
{
	/* Per sincronizzazione con tick a 5 ms usata nel firmware vecchio
	 * Qui la funzione è chiamata ogni 5 ms, quindi non serve sincronizzarsi
	if (MasterFlag.bit.tick_interrupt_wrout == 0)   // non è ora di eseguire
		return;

	MasterFlag.bit.tick_interrupt_wrout = 0;
	MasterFlag.bit.tick_interrupt_rdin = 1;			//sync per lettura input
	*/

	if (outputOK == pdTRUE)
	{
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, phyOut[PA_OUT].bit.b15);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, phyOut[PB_OUT].bit.b2);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, phyOut[PD_OUT].bit.b2);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, phyOut[PE_OUT].bit.b3);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, phyOut[PE_OUT].bit.b4);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, phyOut[PE_OUT].bit.b5);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, phyOut[PE_OUT].bit.b6);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, phyOut[PE_OUT].bit.b7);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, phyOut[PE_OUT].bit.b8);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, phyOut[PE_OUT].bit.b11);

		outputOK = pdFALSE;
	}
}

/**
 * @brief Routine di filtraggio degli input
 *
 * Si occupa di filtrare gli input per dare stabilità alle letture
 */
void	filt_input(uint8_t num_porta, uint16_t	porta_read)
{
		//confronto con lettura precedente (I_in)
	if (porta_read == precIn[num_porta].word)
	{
		if (GESTIO_OROL_INPUT(num_porta) > 10) 		//50 mSec
		{
				// input stabili: aggiorna buffer stato
			GESTIO_OROL_INPUT(num_porta) = 0;
			stato_IO.in.porta[num_porta].word = porta_read;
		}

	}
	else 		// se diversa: azzero orologio e salvo nuova situazione
	{
		GESTIO_OROL_INPUT(num_porta) = 0;
		precIn[num_porta].word = porta_read;
	}
}

/**
 * @brief Routine di inizializzazione del task
 *
 * Fa le inizializzazioni alla partenza del thread
 */
void	initTaskGestIO(void)
{

}



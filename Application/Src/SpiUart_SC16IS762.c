/** ****************************************************************************
	@file		SpiUart_SC16IS762.c
	@brief		Gestione dei chip Spi Uart.
				L'automa si occupa di inizializzare le varie uart disponibili.
				La spedizione verrà eseguita attraverso la chiamata di una routine
				che riempirà la FIFO del chip e restituirà quandi byte è riuscita a spedire
				La ricezione è effettuata direttamente su interrupt e andrà a scrivere
				su un buffer circolare.

	@author
	@version
	@date
******************************************************************************/
#include "appl_types.h"
#include "main.h"
#include "CommonDef.h"
#include "data.h"
#include "defs.h"
#include "macros.h"
//#include "CommonDatiExt.h"
//#include "MasterDatiExt.h"
//#include "GestIO.h"
#include "spi_uart.h"
#include "cmsis_os2.h"
#include "common_tasks.h"
#include "lancom.h"

/*****************************************************************************
	Definizioni locali
******************************************************************************/
AppStatoAutomi_t					automaSpi[NUM_PORTE_COM];
#define ST_CORRENTE(i) 				automaSpi[i].statoCorrente
#define ST_RITORNO(i)	 			automaSpi[i].statoRitorno
#define FLAG_STATO(i)				automaSpi[i].flag.bit.subStato

#define OROL_RST_SPI_UART_5mS		MasterTimer_u16_5mS.rst_spi_uart
#define THIS_STACK_SIZE				DEFAULT_STACK_SIZE
#define OROL_SPI_UART_5mS(i)		MasterTimer_u16_5mS.spi_uart[i]
/*****************************************************************************
	Funzioni locali
******************************************************************************/
void	fsm_ST_COM_INIT(uint8_t index);
void	fsm_ST_COM_SETUP_INIZIALE(uint8_t index);
void	fsm_ST_COM_SETUP_UTENTE(uint8_t index);
void	fsm_ST_COM_SETUP_FINALE(uint8_t index);
void	fsm_ST_COM_READY(uint8_t index);
void	fsm_ST_COM_RX(uint8_t index);
void	fsm_ST_COM_TX(uint8_t index);
void 	initSpiTask(void);
void 	taskSpiUart(void *pvParameters);
/*****************************************************************************
	Variabili locali
******************************************************************************/
uint8_t	SPI_UART_conta_step[NUM_PORTE_COM];
uint8_t	SPI_UART_corrente;

uint8_t	man_uart;		// Se == 0xFF, tutto in automatico. Altrimenti, accedo solo alla uart indicata
static StaticEventGroup_t com2LanEventGroup;


/*****************************************************************
	Costanti  locali
******************************************************************/
const void_funcu8 StatiSpiUart[] =
{
	fsm_ST_COM_INIT,			// 0
	fsm_ST_COM_SETUP_INIZIALE,	// 1
	fsm_ST_COM_SETUP_UTENTE,	// 2
	fsm_ST_COM_SETUP_FINALE,	// 3
	fsm_ST_COM_READY,			// 4
	fsm_ST_COM_RX,				// 5
	fsm_ST_COM_TX				// 6
};

const conf_hw_spi_uart_t	conf_hw_spi_uart[NUM_PORTE_COM] =
{
	{	&(UART_1_CS_GPIO_Port->ODR), UART_1_CS_PIN_INDEX, 0,	&(UART_1_IRQ_GPIO_Port->IDR), UART_1_IRQ_PIN_INDEX	},
	{	&(UART_1_CS_GPIO_Port->ODR), UART_1_CS_PIN_INDEX, 1,	&(UART_1_IRQ_GPIO_Port->IDR), UART_1_IRQ_PIN_INDEX	},
	{	&(UART_2_CS_GPIO_Port->ODR), UART_2_CS_PIN_INDEX, 0,	&(UART_2_IRQ_GPIO_Port->IDR), UART_2_IRQ_PIN_INDEX	},
	{	&(UART_2_CS_GPIO_Port->ODR), UART_2_CS_PIN_INDEX, 1,	&(UART_2_IRQ_GPIO_Port->IDR), UART_2_IRQ_PIN_INDEX	},
	{	&(UART_3_CS_GPIO_Port->ODR), UART_3_CS_PIN_INDEX, 0,	&(UART_3_IRQ_GPIO_Port->IDR), UART_3_IRQ_PIN_INDEX	},
	{	&(UART_3_CS_GPIO_Port->ODR), UART_3_CS_PIN_INDEX, 1,	&(UART_3_IRQ_GPIO_Port->IDR), UART_3_IRQ_PIN_INDEX	},
	{	&(UART_4_CS_GPIO_Port->ODR), UART_4_CS_PIN_INDEX, 0,	&(UART_4_IRQ_GPIO_Port->IDR), UART_4_IRQ_PIN_INDEX	},
	{	&(UART_4_CS_GPIO_Port->ODR), UART_4_CS_PIN_INDEX, 1,	&(UART_4_IRQ_GPIO_Port->IDR), UART_4_IRQ_PIN_INDEX	},
	{	&(UART_5_CS_GPIO_Port->ODR), UART_5_CS_PIN_INDEX, 0,	&(UART_5_IRQ_GPIO_Port->IDR), UART_5_IRQ_PIN_INDEX	},
	{	&(UART_5_CS_GPIO_Port->ODR), UART_5_CS_PIN_INDEX, 1,	&(UART_5_IRQ_GPIO_Port->IDR), UART_5_IRQ_PIN_INDEX	},
};

// Sequenza fissa di configurazione iniziale (vedi datasheet e documento relativo alla sequenza di inizializzazione)
const	passo_conf_reg_spi_uart_t	conf_reg_spi_uart_fissa_iniziale[] =
{
	{	SC16IS762_EFCR,		VAL_EFCR_INI	},
	{	SC16IS762_LCR,		VAL_LCR_INI		},
};

// Sequenza fissa di configurazione finale
const	passo_conf_reg_spi_uart_t	conf_reg_spi_uart_fissa_finale[] =
{
	{	SC16IS762_FCR,		VAL_FCR_FIN		},
	{	SC16IS762_IER,		VAL_IER_FIN		},
	{	SC16IS762_EFCR,		VAL_EFCR_FIN	},
};


const osThreadAttr_t spiTask_attributes = {
  .name = "Spi2Com",
  .stack_size = THIS_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal
};

/*****************************************************************************
	Name:    		SPI_UART_write_reg
	Description:	Esegue la scrittura di un buffer di dati
					su una particolare seriale
*****************************************************************************/
BaseType_t	SPI_UART_write(uint8_t num_com, uint8_t adr_reg, uint8_t *buf_dati_tx, uint8_t	len_dati)
{
	HAL_StatusTypeDef 	res;
	uint16_t			dummy16;
	SpiUartCmd_t		SpiUartCmd;

	dummy16 = hspi1.Instance->DR;
	dummy16 += hspi1.Instance->SR;

	SpiUartCmd.byte = 0;

	if ((len_dati > 0) && (len_dati <= 64 ))
	{
		// CE basso
		Perif_OutBit_BB(conf_hw_spi_uart[num_com].ce_port, conf_hw_spi_uart[num_com].ce_pin) = 0;

		SpiUartCmd.bit.canale	= conf_hw_spi_uart[num_com].canale;
		SpiUartCmd.bit.adr_reg 	= adr_reg;
		SpiUartCmd.bit.r_w 		= SPI_UART_WRITE;

		// Comando
		res = HAL_SPI_Transmit(&hspi1, &SpiUartCmd.byte, 1, 2);

		if (res == HAL_OK)
		{
			// Buffer
			res = HAL_SPI_Transmit(&hspi1, buf_dati_tx, len_dati, 2);
		}

		// CE alto
		Perif_OutBit_BB(conf_hw_spi_uart[num_com].ce_port, conf_hw_spi_uart[num_com].ce_pin) = 1;

		if (res == HAL_OK)
			return(pdTRUE);
	}

	return(pdFALSE);
}


/*****************************************************************************
	Name:    		SPI_UART_read_reg
	Description:	Esegue la lettura di un buffer di dati su una particolare seriale

*****************************************************************************/
BaseType_t	SPI_UART_read(uint8_t num_com, uint8_t adr_reg, uint8_t *buf_dati_rx, uint8_t	len_dati)
{
	HAL_StatusTypeDef 	res;
	uint16_t			dummy16;
	SpiUartCmd_t		SpiUartCmd;

	dummy16 = hspi1.Instance->DR;
	dummy16 += hspi1.Instance->SR;

	SpiUartCmd.byte = 0;

	if ((len_dati > 0) && (len_dati <= 64 ))
	{
		// CE basso
		Perif_OutBit_BB(conf_hw_spi_uart[num_com].ce_port, conf_hw_spi_uart[num_com].ce_pin) = 0;

		SpiUartCmd.bit.canale = conf_hw_spi_uart[num_com].canale;
		SpiUartCmd.bit.adr_reg = adr_reg;
		SpiUartCmd.bit.r_w = SPI_UART_READ;

		// Comando
		res = HAL_SPI_Transmit(&hspi1, &SpiUartCmd.byte, 1, 2);

		if (res == HAL_OK)
		{
			// Buffer
			res = HAL_SPI_Receive(&hspi1, buf_dati_rx, len_dati, 2);
		}

		// CE alto
		Perif_OutBit_BB(conf_hw_spi_uart[num_com].ce_port, conf_hw_spi_uart[num_com].ce_pin) = 1;

		if (res == HAL_OK)
			return(pdTRUE);
	}

	return(pdFALSE);
}

/*****************************************************************************
 *	Task principale di gestione spiUart
*****************************************************************************/
void taskSpiUart(void *pvParameters)
{
	uint8_t i;
	for(;;)
	{
		if (masterCmd.bit.rst_spi_com == 1)
		{
			// Richiesta di reset dei chip SPI_UART (unico seegnale per tutti e 5 i chip)
			masterCmd.bit.rst_spi_com = 0;
			OROL_RST_SPI_UART_5mS = 0;

			MasterFlag.bit.rst_spi_com_in_corso = 1;
			SPI_CMD_UART_RST_BB = 1;
		}

		if (MasterFlag.bit.rst_spi_com_in_corso == 1)
		{
			if (OROL_RST_SPI_UART_5mS > 20)
			{
				MasterFlag.bit.rst_spi_com_in_corso = 0;
				SPI_CMD_UART_RST_BB = 0;
				// Occorre riconfigurare tutte le seriali
				for (i = 0 ; i < NUM_PORTE_COM ; i++)
				{
					ST_CORRENTE(i)	= ST_SPI_UART_INIT;
					FLAG_STATO(i)	= ENTRA_STATO;
				}

				SPI_UART_corrente = 0xFF;	// Così iniziamo a inizializzare l'SPI_UART_corrente = 0;
			}
		}
		else
		{
			if (man_uart == 0xFF)	SPI_UART_corrente++;
			else					SPI_UART_corrente = man_uart;

			if (SPI_UART_corrente >= NUM_PORTE_COM)	SPI_UART_corrente = 0;

			if (ST_CORRENTE(SPI_UART_corrente) > LAST_ST_SPI_UART)	ST_CORRENTE(SPI_UART_corrente) = 0;
			StatiSpiUart[ST_CORRENTE(SPI_UART_corrente)](SPI_UART_corrente);
		}
		osDelay(1);
	}
}

/*******************************************************************************
	AUTOMA:			AutoSpiUart
	STATO:			AutoSpiUart_ST_SPI_UART_INIT
	DESCRIZIONE:	Stato di sola attesa prima della configurazione
*******************************************************************************/
void	fsm_ST_COM_INIT(uint8_t index)
{
	if (FLAG_STATO(index) == ENTRA_STATO)
	{
		FLAG_STATO(index) = EXEC_STATO;
		OROL_SPI_UART_5mS(index) = 0;
	}

	if (OROL_SPI_UART_5mS(index) >= 2)
	{
		FLAG_STATO(index) = ENTRA_STATO;
		ST_CORRENTE(index) = ST_SPI_UART_SETUP_INIZIALE;
		return;
	}
}

/*******************************************************************************
	AUTOMA:			AutomaSpiUart
	STATO:			AutoSpiUart_ST_SPI_UART_SETUP_INIZIALE
	DESCRIZIONE:	Configurazione UART iniziale
*******************************************************************************/
void	fsm_ST_COM_SETUP_INIZIALE(uint8_t index)
{
	const passo_conf_reg_spi_uart_t	*p_conf_reg;

	if (FLAG_STATO(index) == ENTRA_STATO)
	{
		FLAG_STATO(index) = EXEC_STATO;
		OROL_SPI_UART_5mS(index) = 0;
		SPI_UART_conta_step[index] = 0;
	}

	p_conf_reg = &conf_reg_spi_uart_fissa_iniziale[SPI_UART_conta_step[index]];

	SPI_UART_write(index,  p_conf_reg->adr_reg, (uint8_t *)&p_conf_reg->dato, 1);

	SPI_UART_conta_step[index]++;

	if (SPI_UART_conta_step[index] >= (sizeof(conf_reg_spi_uart_fissa_iniziale) / sizeof(passo_conf_reg_spi_uart_t)))
	{
		// Configurazione fissa conclusa
		FLAG_STATO(index) = ENTRA_STATO;
		ST_CORRENTE(index) = ST_SPI_UART_SETUP_UTENTE;
	}
}

/*******************************************************************************
	AUTOMA:			AutomaSpiUart
	STATO:			AutoSpiUart_ST_SPI_UART_SETUP_UTENTE
	DESCRIZIONE:	Configurazione UART utente (solo transizione)
*******************************************************************************/
void	fsm_ST_COM_SETUP_UTENTE(uint8_t index)
{
	uint16_t		divisore;
	uint8_t			l_byte, h_byte;
	reg_lcr_t		reg_lcr;
	cfg_uart_t		*p_cfg_uart;

	p_cfg_uart = &masterParametri.cfg_uart[index];

	// Scrittura registri DLL e DLH per baudrate richiesto
	divisore = DIVISOR_SPI_UART(p_cfg_uart->uart_speed);
	l_byte = (uint8_t)(divisore & 0xFF);
	h_byte = (uint8_t)((divisore >> 8) & 0xFF);

	SPI_UART_write(index,  SC16IS762_DLL, (uint8_t *)&l_byte , 1);
	SPI_UART_write(index,  SC16IS762_DLH, (uint8_t *)&h_byte, 1);

	SPI_UART_read(index, SC16IS762_DLL, &registri_spi_uart[index].DLL, 1);
	SPI_UART_read(index, SC16IS762_DLH, &registri_spi_uart[index].DLH, 1);


	// Altre configurazioni
	reg_lcr.byte = 0;

	reg_lcr.bit.word_len = p_cfg_uart->n_bit - 5;		// Valore compreso tra 3 (8 bit) e 0 (5 bit)

	switch (p_cfg_uart->parity)
	{
		case 'n':	// No parity
			reg_lcr.bit.parity_enable = 0;
			reg_lcr.bit.even_parity = 0;
			break;
		case 'o':	// parity odd
			reg_lcr.bit.parity_enable = 1;
			reg_lcr.bit.even_parity = 0;
			break;
		case 'e':	// parity even
			reg_lcr.bit.parity_enable = 1;
			reg_lcr.bit.even_parity = 1;
			break;
	}

	reg_lcr.bit.stop_bit = p_cfg_uart->nbit_stop;

	SPI_UART_write(index,  SC16IS762_LCR, &reg_lcr.byte, 1);

	FLAG_STATO(index) = ENTRA_STATO;
	ST_CORRENTE(index) = ST_SPI_UART_SETUP_FINALE;
}

/*******************************************************************************
	AUTOMA:			AutomaSpiUart
	STATO:			AutoSpiUart_ST_SPI_UART_SETUP_FINALE
	DESCRIZIONE:	Configurazione UART finale
*******************************************************************************/
void	fsm_ST_COM_SETUP_FINALE(uint8_t index)
{
	const passo_conf_reg_spi_uart_t	*p_conf_reg;

	if (FLAG_STATO(index) == ENTRA_STATO)
	{
		FLAG_STATO(index) = EXEC_STATO;
		OROL_SPI_UART_5mS(index) = 0;
		SPI_UART_conta_step[index] = 0;
	}

	p_conf_reg = &conf_reg_spi_uart_fissa_finale[SPI_UART_conta_step[index]];

	SPI_UART_write(index,  p_conf_reg->adr_reg, (uint8_t *)&p_conf_reg->dato, 1);

	SPI_UART_conta_step[index]++;

	if (SPI_UART_conta_step[index] >= (sizeof(conf_reg_spi_uart_fissa_finale) / sizeof(passo_conf_reg_spi_uart_t)))
	{
		// Configurazione fissa conclusa
		FLAG_STATO(index) = ENTRA_STATO;
		ST_CORRENTE(index) = ST_SPI_UART_READY;
	}
}

/*******************************************************************************
	AUTOMA:			AutomaSpiUart
	STATO:			AutoSpiUart_ST_SPI_UART_READY
	DESCRIZIONE:	UART pronta
*******************************************************************************/
void	fsm_ST_COM_READY(uint8_t index)
{
	CircularBuffer_t *cBuf = (CircularBuffer_t *)&(bufferSpiRx[index]);
	if (FLAG_STATO(index) == ENTRA_STATO)
	{
		//Verifico sempre se sono scattate le condizioni per inviare il buffer alla LAN
		if(lchHasDataToForward(cBuf, index))
			xEventGroupSetBits(com2LanEvGroupHandle, GET_BIT_EVENT(index));

		FLAG_STATO(index) = EXEC_STATO;

		SPI_UART_read(index, SC16IS762_EFCR, &registri_spi_uart[index].EFCR, 1);

		SPI_UART_read(index, SC16IS762_LCR, &registri_spi_uart[index].LCR.byte, 1);

		SPI_UART_read(index, SC16IS762_IER, &registri_spi_uart[index].IER, 1);

		SPI_UART_read(index, SC16IS762_IIR, &registri_spi_uart[index].IIR.byte, 1);
	}

	if (Perif_GetBit_BB(conf_hw_spi_uart[index].irq_port, conf_hw_spi_uart[index].irq_pin) == 0)
	{
		// Se il pin di IRQ è basso, leggo il contenuto del registro IIR ???
		SPI_UART_read(index, SC16IS762_IIR, &registri_spi_uart[index].IIR.byte, 1);
	}

	if (registri_spi_uart[index].IIR.bit.interrupt_status == 0)
	{
		// C'è un interrupt pending
		switch (registri_spi_uart[index].IIR.bit.encoded_interrupt)
		{
			case IIR_RECEIVE_LINE_STATUS_ERROR:
			case IIR_RECEIVE_TIME_OUT:
			case IIR_RHR:
				// Caratteri da ricevere
				FLAG_STATO(index) = ENTRA_STATO;
				ST_CORRENTE(index) = ST_SPI_UART_RX;
				break;
			case IIR_THR:
				/*??? L'interrupt di trasmissione e disattivo! */
				break;
			default:
				/*???*/
				break;

		}
	}
	// Se a questo punto siamo ancora in stato ready faccio un passaggio a TX
	// così vediamo se c'è qualcosa da trasmettere
	if (ST_CORRENTE(index) == ST_SPI_UART_READY)
		ST_CORRENTE(index) = ST_SPI_UART_TX;
}

/*******************************************************************************
	AUTOMA:			AutomaSpiUart
	STATO:			AutoSpiUart_ST_SPI_UART_RX
	DESCRIZIONE:	UART in ricezione
*******************************************************************************/
void	fsm_ST_COM_RX(uint8_t index)
{
	CircularBuffer_t *cBuf = (CircularBuffer_t *)&(bufferSpiRx[index]);
	uint8_t buf_tmp[MAX_SPI_RX_CHARS_ONESHOT];

	if (FLAG_STATO(index) == ENTRA_STATO)
	{
		FLAG_STATO(index) = EXEC_STATO;
	}

	//Controllo che ci siano dei dati da ricevere
	SPI_UART_read(index,  SC16IS762_LSR, &registri_spi_uart[index].LSR.byte, 1);
	if (registri_spi_uart[index].LSR.bit.data_in_receiver == 1)
	{
		// C'e ancora almeno un carattere nella FIFO da ricevere
		MasterTimer_u16_5mS.LanCom[index] = 0;

		// Verifico quanti caratteri ci sono in coda
		SPI_UART_read(index, SC16IS762_RXLVL, &registri_spi_uart[index].RXLVL, 1);

		// Scarico tutti i caratteri senza verificare il registro LSR (parity/frame error ecc)
		SPI_UART_read(index,  SC16IS762_RHR, buf_tmp, registri_spi_uart[index].RXLVL);

		cbPushLinear(cBuf, buf_tmp, registri_spi_uart[index].RXLVL);
	}

	FLAG_STATO(index) = ENTRA_STATO;
	ST_CORRENTE(index) = ST_SPI_UART_READY;
}

/*******************************************************************************
	AUTOMA:			AutomaSpiUart
	STATO:			AutoSpiUart_ST_SPI_UART_TX
	DESCRIZIONE:	UART in trasmissione
*******************************************************************************/
void	fsm_ST_COM_TX(uint8_t index)
{
	const BaseType_t xClearBits = pdTRUE, xWaitAll = pdFALSE;
	uint8_t buffer[MAX_SPI_RX_CHARS_ONESHOT];
	uint16_t len_to_write;
	CircularBuffer_t *cBuf = (CircularBuffer_t *)&(bufferSpiTx[index]);
    EventBits_t uxBits;
	if (FLAG_STATO(index) == ENTRA_STATO)
	{
		uxBits = xEventGroupWaitBits(lan2ComEvGroupHandle, GET_BIT_EVENT(index), xClearBits, xWaitAll, 0);
		if(uxBits != 0)
			FLAG_STATO(index) = EXEC_STATO;
		else
			FLAG_STATO(index) = ESCI_STATO;
	}

	if (FLAG_STATO(index) == EXEC_STATO)
	{
		while(cBuf->count > 0)
		{
			len_to_write = MAX_SPI_RX_CHARS_ONESHOT;
			if(cBuf->count < len_to_write)
				len_to_write = cBuf->count;
			cbPopLinear(cBuf, buffer, len_to_write);
			SPI_UART_write(index, SC16IS762_THR, buffer, len_to_write);
		}
		FLAG_STATO(index) = ESCI_STATO;
	}

	if (FLAG_STATO(index) == ESCI_STATO)
	{
		FLAG_STATO(index) = ENTRA_STATO;
		ST_CORRENTE(index) = ST_SPI_UART_READY;
	}

}

void initSpiTask(void)
{
	// Creazione del nostro gruppo di eventi
	com2LanEvGroupHandle  = xEventGroupCreateStatic( &com2LanEventGroup );

	// Copia della configurazione in RAM
	if (masterParametri_flash.key != 0x55AA)
	{
		// I parametri di funzionamento vengono inizializzati al primo avvio al default
		masterParametri = masterParametri_flash_fab;
	}
	else
	{
		// Copia in ram dei parametri di flash
		masterParametri = masterParametri_flash;
	}

	man_uart = 0xFF; //Impostare tra 0 e NUM_LAN_COM - 1 se si vuole stare su una sola UART per debug

	// Inizializzazione di tutti i buffer di scambio
	for(int i = 0; i < NUM_PORTE_LAN_COM; i++)
	{
		CircularBuffer_t *pBuf = (CircularBuffer_t *)&(bufferSpiRx[i]);
		cbInitialize(pBuf, (uint16_t)DIMBUF_LAN_COM);
		pBuf = (CircularBuffer_t *)&(bufferSpiTx[i]);
		cbInitialize(pBuf, (uint16_t)DIMBUF_LAN_COM);
	}
	masterCmd.bit.rst_spi_com = 1;
}

/**
 * @brief Routine di creazione del thread FreeRTOS
 *
 * Va richiamata in main e riportata in tasks.h
 */
osThreadId_t createTask_Spi2Com(void* arg)
{
	initSpiTask();
	return osThreadNew(taskSpiUart, arg, &spiTask_attributes);
}

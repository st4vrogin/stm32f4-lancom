/** ****************************************************************************
	@file		spi_uart.h
	@brief		Routine per gestione SPI dei chip SC16IS762 di NXP

	@author		SielTre srl
	@version	0.0.1		2023-06-16
******************************************************************************/
#ifndef __SPI_UART_H__
#define __SPI_UART_H__
#include <stdint.h>
#include "appl_types.h"
#include "main.h"
#include "FreeRTOS.h"

extern 	SPI_HandleTypeDef hspi1;


#define UART_1_CS_PIN_INDEX			7
#define UART_2_CS_PIN_INDEX			8
#define UART_3_CS_PIN_INDEX			1
#define UART_4_CS_PIN_INDEX			9
#define UART_5_CS_PIN_INDEX			0

#define UART_1_IRQ_PIN_INDEX		7
#define UART_2_IRQ_PIN_INDEX		6
#define UART_3_IRQ_PIN_INDEX		5
#define UART_4_IRQ_PIN_INDEX		4
#define UART_5_IRQ_PIN_INDEX		3

/* SPI 4  flash ext interface */
#define SPI_UART						SPI1

#define SPI_UART_CLK_ENABLE()			__SPI1_CLK_ENABLE()

#define SPI_UART_SCK_PIN				GPIO_PIN_3
#define SPI_UART_SCK_GPIO_PORT			GPIOB
#define SPI_UART_SCK_GPIO_CLK_ENABLE	__GPIOB_CLK_ENABLE
#define SPI_UART_SCK_AF					GPIO_AF5_SPI1

#define SPI_UART_MISO_PIN				GPIO_PIN_4
#define SPI_UART_MISO_GPIO_PORT			GPIOB
#define SPI_UART_MISO_GPIO_CLK_ENABLE	__GPIOB_CLK_ENABLE
#define SPI_UART_MISO_AF				GPIO_AF5_SPI1

#define SPI_UART_MOSI_PIN				GPIO_PIN_5
#define SPI_UART_MOSI_GPIO_PORT			GPIOB
#define SPI_UART_MOSI_GPIO_CLK_ENABLE	__GPIOB_CLK_ENABLE
#define SPI_UART_MOSI_AF				GPIO_AF5_SPI1

/* define varie */
#define	CLOCK_SPI_UART					3686400		// Hz
#define	DIVISOR_SPI_UART(baudrate)		(CLOCK_SPI_UART / (baudrate * 16))		// Con prescaler = 1 (vedi datasheet pagina 16)

#define	SPI_UART_READ					1
#define	SPI_UART_WRITE					0

/**************************************
	Indirizzi general register set
***************************************/
#define SC16IS762_RHR					0x00
#define SC16IS762_THR					0x00

#define SC16IS762_IER					0x01

#define SC16IS762_FCR					0x02
#define SC16IS762_IIR					0x02

#define SC16IS762_LCR					0x03
#define SC16IS762_MCR					0x04
#define SC16IS762_LSR					0x05
#define SC16IS762_MSR					0x06
#define SC16IS762_SPR					0x07
#define SC16IS762_TXLVL					0x08
#define SC16IS762_RXLVL					0x09
#define SC16IS762_IODir					0x0A
#define SC16IS762_IOState				0x0B
#define SC16IS762_IOIntEna				0x0C
#define SC16IS762_IOControl				0x0D

#define SC16IS762_EFCR					0x0F

// registri con accesso particolare
#define SC16IS762_TCR					0x06
#define SC16IS762_TLR					0x07

/**************************************
	Indirizzi special register set
***************************************/
#define SC16IS762_DLL					0x00
#define SC16IS762_DLH					0x01

/**************************************
	Valori di interrupt del registro
	IIR
***************************************/
#define	IIR_RECEIVE_LINE_STATUS_ERROR	0x03
#define	IIR_RECEIVE_TIME_OUT			0x06
#define	IIR_RHR							0x02
#define	IIR_THR							0x01

/**************************************
	Valori di registri usati
	nell'inizializzazione iniziale
***************************************/
#define	VAL_EFCR_INI					0x06		// disattivazione ricezione e trasmissione
#define	VAL_LCR_INI						0x80		// attivazione latch per il divisore di frequenza

//#define	VAL_FCR_FIN					0xC1		// trigger FIFO RX 60 byte e FIFO enable
#define	VAL_FCR_FIN						0x41		// trigger FIFO RX 16 byte e FIFO enable
#define	VAL_IER_FIN						0x05		// Receive line status register enable && Receive holding register enable
#define	VAL_EFCR_FIN					0x00		// attivazione ricezione e trasmissione

/**************************************
	Definizione stati automa
***************************************/
#define	ST_SPI_UART_INIT			0
#define ST_SPI_UART_SETUP_INIZIALE	1
#define ST_SPI_UART_SETUP_UTENTE	2
#define ST_SPI_UART_SETUP_FINALE	3
#define	ST_SPI_UART_READY			4
#define	ST_SPI_UART_RX				5
#define	ST_SPI_UART_TX				6
#define	LAST_ST_SPI_UART			ST_SPI_UART_TX

typedef struct __attribute__((packed))
{
	volatile uint32_t *		ce_port;		// porta del CE del chip
	uint16_t					ce_pin;			// pin del CE del chip
	uint8_t					canale;			// 0 o 1 corrisponde a quale uart utilizzare relativamente al chip indirizzato

	volatile uint32_t *		irq_port;		// porta del irq del chip
	uint16_t					irq_pin;		// pin del irq del chip
} conf_hw_spi_uart_t;

typedef struct __attribute__((packed))
{
	uint8_t			adr_reg;
	uint8_t			dato;
} passo_conf_reg_spi_uart_t;

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
		uint8_t	unused				:1,
				canale				:2,		// 0 o 1 corrisponde a quale uart utilizzare relativamente al chip indirizzato
				adr_reg				:4,		// Indirizzo del registro da gestire
				r_w					:1;		// 0 = write 1 = read
	}
	bit;
} SpiUartCmd_t;

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
		uint8_t	interrupt_status	:1,
				encoded_interrupt	:5,
				fcr_0				:2;
	}
	bit;
} reg_iir_t;

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
		uint8_t	word_len			:2,
				stop_bit			:1,
				parity_enable		:1,
				even_parity			:1,
				set_parity			:1,
				set_break			:1,
				divisor_latch		:1;
	}
	bit;
} reg_lcr_t;

typedef union __attribute__((packed))
{
	uint8_t	byte;
	struct __attribute__((packed))
	{
		uint8_t	data_in_receiver	:1,
				overrun_error		:1,
				parity_error		:1,
				framing_error		:1,
				break_interrupt		:1,
				THR_empy			:1,
				THR_TSR_empy		:1,
				FIRO_DATA_error		:1;
	}
	bit;
	struct __attribute__((packed))
	{
		uint8_t	data_in_receiver	:1,
				errors				:4,
				others				:3;
	}bit_errors;
} reg_lsr_t;

typedef struct __attribute__((packed))
{
	uint8_t		RHR;
	uint8_t		THR;

	uint8_t		IER;

	uint8_t		FCR;
	reg_iir_t	IIR;

	reg_lcr_t	LCR;
	uint8_t		MCR;
	reg_lsr_t	LSR;
	uint8_t		MSR;
	uint8_t		SPR;
	uint8_t		TXLVL;
	uint8_t		RXLVL;

	uint8_t		IODir;
	uint8_t		IOState;
	uint8_t		IOIntEna;
	uint8_t		IOControl;

	uint8_t		EFCR;
	uint8_t		TCR;
	uint8_t		TLR;

	uint8_t		DLL;
	uint8_t		DLH;
} registri_spi_uart_t;



/* Funzioni esportate */
void	SPI_UART_init(void);
void	AutomaSpiUart(void);

BaseType_t	SPI_UART_write(uint8_t num_com, uint8_t adr_reg, uint8_t *buf_dati_tx, uint8_t	len_dati);
BaseType_t	SPI_UART_read(uint8_t num_com, uint8_t adr_reg, uint8_t *buf_dati_rx, uint8_t	len_dati);

#define MAX_SPI_RX_CHARS_ONESHOT		64

#endif //__SPI_UART_H__

/*
 * mem_siel_rtos.c
 *
 *  Created on: Feb 13, 2026
 *      Author: Siel Tre
 */
#include "FreeRTOS.h"
#include "mem_siel_rtos.h"
#include "CommonDef.h"
#include "sbanc.h"

#define SIEL_FLASH_TIMEOUT      50000U      //Deve essere uguale a FLASH_TIMEOUT_VALUE delle librerie

void *msrMalloc(size_t size)
{
	return pvPortMalloc(size);
}

void *msrCalloc(size_t size)
{
	uint8_t *pBuf = (uint8_t *)msrMalloc(size);
	if(pBuf == NULL)
		return NULL;
	for(int i = 0; i < size; ++i)
		pBuf[i] = 0;
	return (void *)pBuf;
}

void *msrRealloc(void* orig, size_t newSize)
{
	size_t i;
	uint8_t *newPtr, *punt_buf;
	if (orig == NULL)
		return NULL;
	newPtr = (uint8_t*)msrMalloc(newSize);
	if(newPtr == NULL)
		return NULL;
	punt_buf = (uint8_t *)orig;
	for (i = 0; i < newSize; ++i)
	{
		newPtr[i] = punt_buf[i];
	}
	msrFree(orig);
	return (void *)newPtr;
}

void msrFree(void *ptr) { vPortFree(ptr); }

int8_t  	msrGetMemType(uint32_t adr)
{
	uint32_t		len_flash = FLASH_SIZE->flash_size * 1024;
	uint32_t		len_ram = SRAM_196K;

	if ((adr < len_flash) || 												//	0x00000000 - 0x000xxxxx	zona rimappata di flash
		((adr >= FLASH_BASE) && (adr < (FLASH_BASE + len_flash))))			//	0x08000000 - 0x080xxxxx	zona di flash
	{
		return MEM_ID_FLASH;
	}
	else if ((adr >= SRAM_BASE) && (adr < (SRAM_BASE + len_ram)))			//	0x20000000 - 0x2000xxxx	zona di SRAM
	{
		return MEM_ID_SRAM;
	}
	else if (	((adr >= APB1PERIPH_BASE) && (adr < (APB1PERIPH_BASE + 0x80000)))	||		// APB1PERIPH_BASE, APB2PERIPH_BASE, AHB1PERIPH_BASE
				((adr >= AHB2PERIPH_BASE) && (adr < (AHB2PERIPH_BASE + 0x60C00)))			// AHB2PERIPH_BASE
			)
	{
		// Attenzione: in questo range di indirizzi, alcune range di indirizzi non esistono!!!
		return MEM_ID_REG;
	}
	else if ((adr >= FLASH_EXT_SPI_BASE) && (adr < FLASH_EXT_SPI_BASE + LEN_FLASH_EXT_SPI))
	{
		return MEM_ID_FLASH_EXT;
	}
	else
	{
		return MEM_ID_UNKNOWN;
	}
}

const	uint32_t	addr_end_sector[FLASH_SECTOR_TOTAL]=
{
	0x08003FFF,		//!< 00		 16 KB
	0x08007FFF,		//!< 01		 16 KB
	0x0800BFFF,		//!< 02		 16 KB
	0x0800FFFF,		//!< 03		 16 KB
	0x0801FFFF,		//!< 04		 64 KB
	0x0803FFFF,		//!< 05		128 KB
	0x0805FFFF,		//!< 06		128 KB
	0x0807FFFF,		//!< 07		128 KB
	0x0809FFFF,		//!< 08		128 KB
	0x080BFFFF,		//!< 09		128 KB
	0x080DFFFF,		//!< 10		128 KB
	0x080FFFFF,		//!< 11		128 KB

	0x08103FFF,		//!< 12		 16 KB
	0x08107FFF,		//!< 13		 16 KB
	0x0810BFFF,		//!< 14		 16 KB
	0x0810FFFF,		//!< 15		 16 KB
	0x0811FFFF,		//!< 16		 64 KB
	0x0813FFFF,		//!< 17		128 KB
	0x0815FFFF,		//!< 18		128 KB
	0x0817FFFF,		//!< 19		128 KB
	0x0819FFFF,		//!< 20		128 KB
	0x081BFFFF,		//!< 21		128 KB
	0x081DFFFF,		//!< 22		128 KB
	0x081FFFFF,		//!< 23		128 KB
};

/*****************************************************************
	siel_flash_program_u32
	Routine di scrittura in flash per STM32F0

	Parametri:
		uint32 dest			Indirizzo di destinazione
		uint32 *punt_src	Puntatore all'indirizzo sorgente
		uint32 len_src		Quantità dati da scrivere

	Ritorno:
		tipo FLASH_Status	Vedi file .H
******************************************************************/
HAL_StatusTypeDef msrFlashWrite_u32(uint32_t dest, uint32_t *punt_src, uint32_t len_src)
{
	HAL_StatusTypeDef	status = HAL_OK;
	uint32_t			i;
	uint32_t			*punt_dest = (uint32_t *)dest;

	HAL_FLASH_Unlock();

	/* Clear pending flags (if any) */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP |
							FLASH_FLAG_OPERR |
							FLASH_FLAG_WRPERR |
							FLASH_FLAG_PGAERR |
							FLASH_FLAG_PGPERR |
							FLASH_FLAG_PGSERR |
							FLASH_FLAG_RDERR |
							FLASH_FLAG_BSY);

	/* Wait for last operation to be completed */
	status = FLASH_WaitForLastOperation(SIEL_FLASH_TIMEOUT);

	if(status == HAL_OK)
	{
		/* If the previous operation is completed, proceed to program half word */
		FLASH->CR &= CR_PSIZE_MASK;
		FLASH->CR |= FLASH_PSIZE_WORD;
		FLASH->CR |= FLASH_CR_PG;

		for (i = 0 ; i < (len_src / sizeof(uint32_t)) ; i++)
		{
			if (punt_dest[i] != (uint32_t)BLANK_FLASH)
			{
				status = HAL_ERROR;
				break;
			}
			punt_dest[i] = punt_src[i];

			/* Wait for last operation to be completed */
			status = FLASH_WaitForLastOperation(SIEL_FLASH_TIMEOUT);

			if(status != HAL_OK)
			{
				break;				// Scrittura non completata
			}
		}

		/* Disable the PG Bit */
		FLASH->CR &= ~FLASH_CR_PG;
	}

	HAL_FLASH_Lock();

	/* Return the Program Status */
	return status;
}

/*****************************************************************
	siel_flash_program_u8
	Routine di scrittura in flash per STM32F0

	Parametri:
		uint32 dest			Indirizzo di destinazione
		uint8 *punt_src		Puntatore all'indirizzo sorgente
		uint32 len_src		Quantità dati da scrivere

	Ritorno:
		tipo FLASH_Status	Vedi file .H
******************************************************************/
HAL_StatusTypeDef msrFlashWrite_u8(uint32_t dest, uint8_t *punt_src, uint32_t len_src)
{
	HAL_StatusTypeDef	status = HAL_OK;
	uint32_t			i;
	uint8_t			*punt_dest = (uint8_t *)dest;

	HAL_FLASH_Unlock();

	/* Clear pending flags (if any) */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP |
							FLASH_FLAG_OPERR |
							FLASH_FLAG_WRPERR |
							FLASH_FLAG_PGAERR |
							FLASH_FLAG_PGPERR |
							FLASH_FLAG_PGSERR |
							FLASH_FLAG_RDERR |
							FLASH_FLAG_BSY);

	/* Wait for last operation to be completed */
	status = FLASH_WaitForLastOperation(SIEL_FLASH_TIMEOUT);

	if(status == HAL_OK)
	{
		/* If the previous operation is completed, proceed to program half word */
		FLASH->CR &= CR_PSIZE_MASK;
		FLASH->CR |= FLASH_PSIZE_BYTE;
		FLASH->CR |= FLASH_CR_PG;

		for (i = 0 ; i < len_src ; i++)
		{
			if (punt_dest[i] != (uint8_t)BLANK_FLASH)
			{
				status = HAL_ERROR;
				break;
			}
			punt_dest[i] = punt_src[i];

			/* Wait for last operation to be completed */
			status = FLASH_WaitForLastOperation(SIEL_FLASH_TIMEOUT);

			if(status != HAL_OK)
			{
				break;				// Scrittura non completata
			}
		}

		/* Disable the PG Bit */
		FLASH->CR &= ~FLASH_CR_PG;
	}

	HAL_FLASH_Lock();

	/* Return the Program Status */
	return status;
}

/*****************************************************************
	siel_flash_erase
	Routine di cancellazione flash per STM32F0

	Parametri:
		uint32 dest			Indirizzo di destinazione
		uint32 len_src		Quantità dati da cancellare
	Ritorno:
		SectorError: pointer to variable  that
        contains the configuration information on faulty sector in case of error
        (ERASE_FLASH_OK=0xFFFFFFFF means that all the sectors have been correctly erased)
******************************************************************/
uint32_t msrFlashErase(uint32_t dest, uint32_t len_src)
{
	uint8_t					i, sector_start, sector_end;
	FLASH_EraseInitTypeDef	EraseInit;
	uint32_t				errore;

	// Cancellazione banco
	for (i = 0 ; i < FLASH_SECTOR_TOTAL ; i++)
	{
		if (dest <= addr_end_sector[i])	break;
	}
	sector_start = i;

	for (i = 0 ; i < FLASH_SECTOR_TOTAL ; i++)
	{
		if ((dest + len_src - 1)  <= addr_end_sector[i])	break;
	}
	sector_end = i;

	EraseInit.TypeErase = TYPEERASE_SECTORS;
	EraseInit.Banks = 0U;
	EraseInit.Sector = sector_start;
	EraseInit.NbSectors = sector_end - sector_start + 1;
	EraseInit.VoltageRange = VOLTAGE_RANGE_1;

	HAL_FLASH_Unlock();

	HAL_FLASHEx_Erase(&EraseInit, &errore);
	HAL_FLASH_Lock();

	return errore;
}


/*****************************************************************
	F0_ob_rdp_config
	Routine per modificare i bit rdp

	Parametri:
		uint8 level			Livello di protezione desiderato
	Ritorno:
		tipo FLASH_Status	Vedi file .H
******************************************************************/
//HAL_StatusTypeDef F0_ob_rdp_config(uint8 level)
//{
//	HAL_StatusTypeDef status;
//
//	// Sblocco l'accesso alla flash
//	HAL_FLASH_Unlock();
//
//	// Sblocco l'accesso al registro OB
//	FLASH_OB_Unlock();
//
//	status = FLASH_OB_RDPConfig(level);
//
//	// Blocco l'accesso al registro OB
//	FLASH_OB_Lock();
//
//	/* Return the Erase Status */
//	return status;
//}


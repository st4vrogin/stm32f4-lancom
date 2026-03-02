/****************************************************************************
 * Copyright (C) 2024 Siel Tre                                              *
 *                                                                          *
 * This file is part of TSCube                                              *
 *                                                                          *
 *   TSCube is not free software: you must ask for permission in order      *
 *   to view, use or modify this file. Any misbehavior will be prosecuted.  *
 *                                                                          *
 *   Contacts:                                                              *
 *                                                                          *
 *   Website: https://www.sieltre.it                                        *
 *   E-Mail: info@sieltre.it                                                *
 *                                                                          *
 *                                                                          *
 ****************************************************************************/

/**
 * @file sbanc.h
 * @author Siel Tre
 * @brief Implementazione delle funzioni di sbancamento
*/

#include "common_data.h"
#include "data.h"
#include <string.h>
#include "mem_siel_rtos.h"

/**
 * @brief Legge i dati di configurazione comune (parametri LAN) dall'area parametri in flash e li memorizza in un area in RAM
*/
void common_flash_to_ram()
{
    memcpy(param_sbanc_ram, (const uint8_t*)&fabParametri, sizeof(fabParametri));
    memcpy(&param_sbanc_ram[sizeof(fabParametri)], (const uint8_t*)&commonParametri_flash_1, sizeof(commonParametri_flash_1));
    memcpy(&param_sbanc_ram[sizeof(fabParametri)+sizeof(commonParametri_flash_1)], (const uint8_t*)&commonParametri_flash_2, sizeof(commonParametri_flash_2));
}

/**
 * @brief Scrive i dati di configurazione comune (parametri LAN) dall'area di temporanea di sbancamento alla posizione definitiva in flash
*/
void common_ram_to_flash()
{
    msrFlashErase((uint32_t)&fabParametri, 1);
    msrFlashWrite_u8((uint32_t)&fabParametri, param_sbanc_ram, sizeof(fabParametri));
    msrFlashWrite_u8((uint32_t)&commonParametri_flash_1, &param_sbanc_ram[sizeof(fabParametri)], sizeof(commonParametri_flash_1));
    msrFlashWrite_u8((uint32_t)&commonParametri_flash_2, &param_sbanc_ram[sizeof(fabParametri)+sizeof(commonParametri_flash_1)], sizeof(commonParametri_flash_2));
    // msrFlashWrite_u8((uint32)&masterParametri_flash_fab,
    //     &param_sbanc_ram[sizeof(fabParametri)+sizeof(commonParametri_flash_1)+sizeof(commonParametri_flash_2)], sizeof(masterParametri_flash_fab));
    // msrFlashWrite_u8((uint32)&masterParametri_flash,
    //     &param_sbanc_ram[sizeof(fabParametri)+sizeof(commonParametri_flash_1)+sizeof(commonParametri_flash_2)+sizeof(masterParametri_flash_fab)], sizeof(masterParametri_flash));
}

/**
 * @brief Legge i dati di configurazione applicativo (es. parametri seriale) dall'area parametri in flash e li memorizza in un area in RAM
*/
void app_prm_flash_to_ram()
{
    //memcpy(param_sbanc_ram, (const uint8*)&masterParametri_flash_fab, sizeof(masterParametri_flash_fab));
    //memcpy(&param_sbanc_ram[sizeof(masterParametri_flash_fab)], (const uint8*)&masterParametri_flash, sizeof(masterParametri_flash));
    memcpy(param_sbanc_ram, (const uint8_t*)&masterParametri_flash, sizeof(masterParametri_flash));

}

/**
 * @brief Scrive i dati di configurazione applicativo (es. parametri seriale) dall'area di temporanea di sbancamento alla posizione definitiva in flash
*/
void app_prm_ram_to_flash()
{
    msrFlashErase((uint32_t)&masterParametri_flash, 1);
    msrFlashWrite_u8((uint32_t)&masterParametri_flash, param_sbanc_ram, sizeof(masterParametri_flash));
    //msrFlashWrite_u8((uint32)&masterParametri_flash, &param_sbanc_ram[sizeof(masterParametri_flash_fab)], sizeof(masterParametri_flash));
}

/**
 * @brief Realizzazione custom della funzione memcpy_s
 *
 * La funzione memcpy_s non è supportata dallo standard C99 usata per questo firmware. Le scansioni SAST evidenziano però come
 * problematico l'utilizzo della funzione standard memcpy. Si procede dunque ad implementare un clone di memcpy_s
 *
 * @param dest Buffer di destinazione
 * @param destsz Dimensione (in byte) del buffer di destinazione
 * @param src Puntatore al buffer sorgente
 * @param count Dimensione (in byte) da copiare dal buffer sorgente
*/
void *memcpy_secure(void *restrict dest, size_t destsz, const void *restrict src, size_t count)
{
	size_t copy_size;
	if(count <= destsz)
		copy_size = count;
	else
		copy_size = destsz;
	if(copy_size > 0)
		memcpy(dest, src, copy_size);
	return dest;

}


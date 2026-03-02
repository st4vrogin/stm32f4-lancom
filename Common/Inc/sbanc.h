/****************************************************************************
 * Copyright (C) 2024 Siel Tre                                              *
 *                                                                          *
 * This file is part of TSCube                                              *
 *                                                                          *
 *   TSCube is  not free software: you must ask for permission in order     *
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
 * @brief Definizione delle funzioni di sbancamento
*/
#ifndef _SBANC_H_
#define _SBANC_H_

void common_flash_to_ram(void);
void common_ram_to_flash(void);
void app_prm_flash_to_ram(void);
void app_prm_ram_to_flash(void);

#endif

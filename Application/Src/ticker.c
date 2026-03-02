/*
 * application ticker.c
 *
 *  Created on: Feb 26, 2026
 *      Author: SielTre
 */
#include "data.h"


void ApplicationTicker()
{
	uint16_t i;
	MasterTimer_ticks.t1mS++;
	if (MasterTimer_ticks.t1mS >= 5)
	{
		MasterTimer_ticks.t1mS = 0;
		MasterTimer_ticks.t5mS++;
		for (i = 0 ; i < (sizeof(systmr_b_5ms) / 2); i++) ((uint16_t *)&systmr_b_5ms)[i]++;
		for (i = 0 ; i < (sizeof(MasterTimer_u16_5mS_t) / 2) ; i++) ((uint16_t *)&MasterTimer_u16_5mS)[i]++;
		MasterTimer_ticks.t_lampeggio++;
		if (MasterTimer_ticks.t_lampeggio >= 100)
		{
			MasterTimer_ticks.t_lampeggio = 0;
			LAMPEGGIO = ~LAMPEGGIO;
			LED_PCS_BB = LAMPEGGIO;
			//OUT_FRONT_1_BB = ~LAMPEGGIO;	// E' il negato di LED_PCS_BB
		}
	}

	if (MasterTimer_ticks.t5mS >= 2)
	{
		MasterTimer_ticks.t5mS = 0;

		for (i = 0 ; i < (sizeof(MasterTimer_u16_10mS_t) / 2) ; i++) ((uint16_t *)&MasterTimer_u16_10mS)[i]++;
		MasterTimer_ticks.t10mS++;
	}

	if (MasterTimer_ticks.t10mS >= 10)
	{
		MasterTimer_ticks.t10mS = 0;
		for (i = 0 ; i < (sizeof(MasterTimer_u16_100mS_t) / 2) ; i++) ((uint16_t *)&MasterTimer_u16_100mS)[i]++;
		MasterTimer_ticks.t100mS++;
		//for (i = 0 ; i < (sizeof(systmr_b_100ms) / 2); i++) ((uint16_t *)&systmr_b_100ms)[i]++;
	}

	if (MasterTimer_ticks.t100mS >= 10)
	{
		MasterTimer_ticks.t100mS = 0;
		//incrementa orologi 1sec
		for (i = 0 ; i < (sizeof(MasterTimer_u16_1S_t) / 2); i++) ((uint16_t *)&MasterTimer_u16_1S)[i]++;
		for (i = 0 ; i < (sizeof(MasterTimer_u32_1S_t) / 4); i++) ((uint32_t *)&MasterTimer_u32_1S)[i]++;
		MasterTimer_ticks.t1S++;
	}
	buf_ping_tx_0.uptime = MasterTimer_u32_1S.uptime;
}

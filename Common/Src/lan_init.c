/*
 * lan_init.c
 *
 *  Created on: Feb 20, 2026
 *      Author: Siel Tre
 */
#include "common_data.h"
#include "FreeRTOS_IP.h"
#include "stm32f4xx_hal.h"
#include "common_tasks.h"
#include "lan_init.h"

extern RNG_HandleTypeDef hrng; // Dichiarata solitamente in main.c da CubeMX

static const uint8_t dns_ip_addr[4] = { 8, 8, 8, 8 };
extern osThreadId_t cmTasksIdList[30];

void liInitParametersRam(void)
{

	// Verifica configurazione parametri
	if (commonParametri_flash_1.key == 0x55AA)
	{
		// Normalmente utilizzero sempre la sezione 1
		commonParametri = commonParametri_flash_1;
	}
	else if (commonParametri_flash_2.key == 0x55AA)
	{
		// Copia in ram dei parametri di flash della sezione 2
		commonParametri = commonParametri_flash_2;
	}
}

void liFreeRtosIpInit(void)
{
	liInitParametersRam();
	FreeRTOS_IPInit(commonParametri.ip_addr,
				    commonParametri.netmask_addr,
					commonParametri.gw_addr,
					dns_ip_addr,
					fabParametri.mac_address);
}

void liFreeRtosIpReinit(void)
{
	uint32_t ulNewIpAddr = FreeRTOS_inet_addr_quick(commonParametri.ip_addr[0], commonParametri.ip_addr[1], commonParametri.ip_addr[2], commonParametri.ip_addr[3]);
	uint32_t ulNewMask = FreeRTOS_inet_addr_quick(commonParametri.netmask_addr[0], commonParametri.netmask_addr[1], commonParametri.netmask_addr[2], commonParametri.netmask_addr[3]);
	uint32_t ulNewGw = FreeRTOS_inet_addr_quick(commonParametri.netmask_addr[0], commonParametri.netmask_addr[1], commonParametri.netmask_addr[2], commonParametri.netmask_addr[3]);
	uint32_t ulNewDns = FreeRTOS_inet_addr_quick(dns_ip_addr[0], dns_ip_addr[1], dns_ip_addr[2], dns_ip_addr[3]);
	liStopServers();
	osDelay(1000);
	FreeRTOS_SetAddressConfiguration(&ulNewIpAddr,&ulNewMask,&ulNewGw,&ulNewDns);
	FreeRTOS_NetworkDown();
	osDelay(1000);
	liStartServers();
}

void liStopServers(void)
{
	if(pApplServersStop != NULL)
		pApplServersStop();
	for(int i = 0; i < NUM_GES_COM_LAN; i++)
	{
		lgSuspendTask(i);
		lgCloseSocket(i);
		osDelay(100);
	}
}

void liStartServers(void)
{
	for(int i = 0; i < NUM_GES_COM_LAN; i++)
	{
		lgResumeTask(i);
	}
	if(pApplServersStart != NULL)
		pApplServersStart();
}


/**
 * @brief Implementazione della routine di generazione di numeri casuali, necessaria per FreeRTOS
 */
BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
    if( HAL_RNG_GenerateRandomNumber( &hrng, pulNumber ) == HAL_OK )
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

/**
 * @brief Implementazione della seconda routine di generazione di numeri casuali, necessaria per FreeRTOS
 */
uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                             uint16_t usSourcePort,
                                             uint32_t ulDestinationAddress,
                                             uint16_t usDestinationPort )
{
    uint32_t ulReturn;

    /* Per i numeri di sequenza TCP, la casualità hardware è l'opzione più sicura.
       I parametri passati (indirizzi e porte) sono ignorati se usiamo l'RNG. */
    if( HAL_RNG_GenerateRandomNumber( &hrng, &ulReturn ) == HAL_OK )
    {
        return ulReturn;
    }

    /* Fallback in caso di errore RNG (molto raro su STM32) */
    return 0;
}

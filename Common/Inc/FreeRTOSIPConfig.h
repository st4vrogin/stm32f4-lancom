/*
 * FreeRTOSIPConfig.h
 *
 *  Created on: Feb 16, 2026
 *      Author: Siel Tre
 */

#ifndef FREERTOS_IP_CONFIG_H
#define FREERTOS_IP_CONFIG_H

#define ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES		1 /* era 1 */
#define ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM			1 /* era 1 */
#define ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM			1 /* era 1 */
#define ipconfigZERO_COPY_RX_DRIVER						1
#define ipconfigZERO_COPY_TX_DRIVER						1
#define ipconfigUSE_LINKED_RX_MESSAGES					1
#define ipconfigUSE_RMII					           	0
#define ipconfigNETWORK_MTU           		544
//#define ipconfigTCP_MSS               	512
#define ipconfigTCP_MSS 					( ipconfigNETWORK_MTU - ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER ) )
#define ipconfigTCP_TX_BUFFER_LENGTH	( 16 * ipconfigTCP_MSS )
#define ipconfigTCP_RX_BUFFER_LENGTH	( 16 * ipconfigTCP_MSS )
#define ipconfigBYTE_ORDER				pdFREERTOS_LITTLE_ENDIAN
#define ipconfigUSE_IPv6								0
#define ipconfigUSE_LLMNR								0
#define ipconfigUSE_NBNS								0
#define ipconfigUSE_DHCP								0
#define ipconfigUSE_DNS									0
#define ipconfigSUPPORT_OUTGOING_PINGS					1
#define ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS			40
#define ipconfigTCP_LISTEN_SELF_ABORT_DELAY     		( pdMS_TO_TICKS( 10000 ) )
#define ipconfigTCP_MAX_RETRANSMISSIONS					3
#define ipconfigTCP_KEEP_ALIVE							1
#define ipconfigTCP_KEEP_ALIVE_INTERVAL					10
//#define ipconfigSOCK_DEFAULT_RECEIVE_BLOCK_TIME			( pdMS_TO_TICKS (100) )
#define ipconfigSUPPORT_SIGNALS							1
#define ipconfigSUPPORT_SELECT_FUNCTION					1
#define ipconfigHAS_DEBUG_PRINTF						0
//#define FreeRTOS_debug_printf( X ) 						printf X


#endif /* FREERTOS_IP_CONFIG_H */

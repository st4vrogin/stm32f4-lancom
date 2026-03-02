/*
****************************************************************************
* AutomaPing.c
*
* Versione: 1.0
* Societa: Siel Tre
* Progetto: VoIP BOX
* Data: 31/08/2015 11:33:38
*
* Descrizione: Implementazione dell'automa PING
*
*
****************************************************************************
*/

#ifndef AUTOMA_PING_COMPILE
#define AUTOMA_PING_COMPILE
#endif

#include "data.h"
#include "PingHlp.h"
#include <string.h>
#include "cmsis_os2.h"
#include "FreeRTOS_sockets.h"
#include "FreeRTOS_IP.h"
#include "cmsis_os2.h"

#define MAX_PING_RETRIES		5
#define PING_RETRY_INTERVAL		10		//Equivale a un secondo in quanto si usa timer da 100ms
#define DEF_PING_TTL			32		//Time to live del pacchetto ICMP Echo Req in uscita

/****************************************************************************
* Predichiarazione delle funzioni locali
****************************************************************************/
//void icmp_checksum_calc(struct icmp_echo_hdr *, const uint8_t*, uint8_t);
void ping_set_timeouts_and_stats(void);
void update_ping_elem(au_ping_elem_t*);
void AuPing_stInit(void);
void AuPing_stEnqueueSend(void);
void AuPing_stWaitRetry(void);
void AuPing_stReportResults(void);

/****************************************************************************
*  Variabili e costanti condivise
****************************************************************************/

ping_report_t	ping_report;
uint32_t	ip_addr_to_ping;


/****************************************************************************
*  Variabili e costanti locali
****************************************************************************/
static	uint8_t		max_ping_retries 		= MAX_PING_RETRIES;
static	uint8_t 	abilita_automa_ping 	= 0;
static	uint8_t 	au_ping_cur_stato		= 0;
static	uint8_t		cur_retries				= 0;

#define ICMP_PING_DATA_LEN			24
#if ICMP_PING_DATA_LEN > PING_MAX_DATA
#error "In Automa Ping i dati del pacchetto non possono superare il valore PING_MAX_DATA (definito in ping_hlp.h)"
#endif

#define IP_PING_PAYLOAD_LEN			ICMP_PING_DATA_LEN + 8
#define DEFAULT_ICMP_ID				0x55AA
static	au_ping_elem_t AuPing[MAX_PING_RETRIES];
#define THIS_STACK_SIZE				512

/****************************************************************************
* Definizione stati
****************************************************************************/

#ifdef ST_INIT
#undef ST_INIT
#endif

#define ST_INIT					0
#define ST_ENQUEUE_SEND			1
#define ST_WAIT_RETRY			2
#define ST_REPORT_RESULTS		3

#define AU_CUR_STATO			au_ping_cur_stato

const osThreadAttr_t automaPing_attributes = {
  .name = "auPing",
  .stack_size = THIS_STACK_SIZE,
  .priority = (osPriority_t) osPriorityNormal
};


const void_func StatiPing[] =
{
	AuPing_stInit,			// 0
	AuPing_stEnqueueSend,	// 1
	AuPing_stWaitRetry,	// 2
	AuPing_stReportResults,	// 3
};


void taskAutomaPing(void* pvArguments)
{
	for(;;)
	{
		StatiPing[AU_CUR_STATO]();
		osDelay(1);
	}
}


void AuPing_stInit()
{
	if(abilita_automa_ping)
	{
		//seq_counter = 0;
		cur_retries = 0;
		//IP4_ADDR(&ip_addr_to_ping, 10,50,50,183);
		memset(&ping_report, 0, sizeof(ping_report_t));
		AU_CUR_STATO = ST_ENQUEUE_SEND;
	}
}

void AuPing_stEnqueueSend()
{
	MasterTimer_u16_100mS.PingTimer_100ms = 0;
	uint16_t usRequestSequenceNumber = FreeRTOS_SendPingRequest(ip_addr_to_ping, ICMP_PING_DATA_LEN, pdMS_TO_TICKS(50) );
	if( usRequestSequenceNumber == pdFAIL )
	{
		AuPing[cur_retries].status = PING_ERROR;
		AuPing[cur_retries].seqno = 0xFFFF;
		AuPing[cur_retries].time_sent = 0xFFFF;
		printf("Ping %hu failure\n", cur_retries);
	}
	else
	{
		AuPing[cur_retries].status = PING_SENT;
		AuPing[cur_retries].seqno =  usRequestSequenceNumber;
		AuPing[cur_retries].time_sent = MasterTimer_u16_1mS.PingTimer_1ms;
		//printf("Ping %hu sent\n", cur_retries);
	}
	AU_CUR_STATO = ST_WAIT_RETRY;
}

void AuPing_stWaitRetry()
{
	if(MasterTimer_u16_100mS.PingTimer_100ms == PING_RETRY_INTERVAL)
	{
		if(cur_retries == max_ping_retries - 1)
		{
			AU_CUR_STATO = ST_REPORT_RESULTS;
		}
		else
		{
			AU_CUR_STATO = ST_ENQUEUE_SEND;
			cur_retries++;
		}
	}
}

void AuPing_stReportResults()
{
		if(!ping_report.report_ok)
		{
			ping_set_timeouts_and_stats();
			memset(AuPing,0,sizeof(au_ping_elem_t)*MAX_PING_RETRIES);
		}
		if(abilita_automa_ping == 0)
		{
			AU_CUR_STATO = ST_INIT;	
		}
		cur_retries = 0;
}


void update_ping_elem(au_ping_elem_t* pe) //Da chiamare se il ping e' andato bene
{
	uint16_t now = MasterTimer_u16_1mS.PingTimer_1ms;
	if(now < pe->time_sent)
	{
		pe->rtt = (0xFFFF - pe->time_sent) + now;	
	}
	else
		pe->rtt = now - pe->time_sent;
	pe->status = PING_OK;
}

void ping_set_timeouts_and_stats()
{
	uint8_t i, conta_ok = 0, conta_tutto = 0;
	uint16_t min, max, avg;
	min = 0xFFFF;
	max = 0;
	avg = 0;
	for (i=0; i < max_ping_retries; i++)
	{
		if(AuPing[i].status != PING_OK && AuPing[i].status == PING_SENT)
		{
			AuPing[i].status = PING_TIMEOUT;
			printf("Ping %d reply timeout. seqno: %d\n", i, AuPing[i].seqno);
			conta_tutto++;
		}
		else if (AuPing[i].status == PING_OK)
		{
			if(AuPing[i].rtt < min)
				min = AuPing[i].rtt;
			if(AuPing[i].rtt > max)
				max = AuPing[i].rtt;
			avg += AuPing[i].rtt;
			conta_ok++;
			conta_tutto++;
		}
		else
		{
			
		}
	}
	if(conta_ok)
	{
		avg = avg / conta_ok;	
	}
	ping_report.pkt_sent = conta_tutto;
	ping_report.pkt_loss = conta_tutto - conta_ok;
	ping_report.max = max;
	ping_report.min = min;
	ping_report.avg = avg;
	ping_report.report_ok = 1;
}

void start_ping(uint8_t* ip, uint8_t nRetries)
{
	if(abilita_automa_ping)
	{
		abilita_automa_ping = 0;
		AU_CUR_STATO = 0;	
	}
	max_ping_retries = nRetries;
	ip_addr_to_ping = FreeRTOS_inet_addr_quick(ip[0],ip[1],ip[2],ip[3]);
	abilita_automa_ping = 1;
}

void stop_ping()
{
	abilita_automa_ping = 0;
}


/**
 * It's an application defined hook (or callback) function
 * that is called by the TCP/IP stack when the stack receives a reply to
 * an ICMP echo (ping) request that was generated using
 * the FreeRTOS_SendPingRequest() function
 */
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier )
{
	int cnt = 0;
	switch( eStatus )
	{
		case eSuccess:
			/* A valid ping reply has been received. */
			while(cnt < MAX_PING_RETRIES)
			{
				if(AuPing[cnt].seqno == usIdentifier && AuPing[cnt].status == PING_SENT)
				{
					printf("Ping %d reply received. seqno: %d\n", cnt, AuPing[cnt].seqno);
					update_ping_elem(&AuPing[cnt]);
					return;
				}
				cnt++;
			}
			break;

		case eInvalidChecksum :
		case eInvalidData :
			/* A reply was received but it was not valid. */
			break;
	}
}

/**
 * @brief Routine di creazione del thread FreeRTOS
 *
 * Va richiamata in main e riportata in tasks.h
 */
osThreadId_t createTask_AutomaPing(void* arg)
{
	return osThreadNew(taskAutomaPing, arg, &automaPing_attributes);
}

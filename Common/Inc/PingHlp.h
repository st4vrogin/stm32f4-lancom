/*****************************************************************************
* PingHlp.h
*
* Versione: 1.0
* Societa: SielTre Srl
* Progetto: VoIP BOX
* Data: 31/08/2015 14:36:01
*
* Descrizione: Strutture di supporto all'automa Ping
*
*
*****************************************************************************/

#ifndef __PING_HLP_H__
#define __PING_HLP_H__

#define PING_MAX_DATA		32

#define PING_ST_UNKNOWN		0
#define PING_SENT			1
#define	PING_OK				2
#define PING_TIMEOUT		4
#define PING_ERROR			5

typedef struct __attribute__((packed))
{
	uint8_t status;
	uint16_t seqno;
	uint16_t time_sent;
	uint16_t rtt;
} au_ping_elem_t;

typedef struct __attribute__((packed))
{
	uint8_t report_ok;
	uint8_t pkt_sent;
	uint8_t pkt_loss;
	uint16_t max;
	uint16_t min;
	uint16_t avg;
} ping_report_t;

void stop_ping(void);
void start_ping(uint8_t*, uint8_t);
void AutomaPing(void);

#ifndef AUTOMA_PING_COMPILE
extern ping_report_t	ping_report;
#endif

#endif //__PING_HLP_H__

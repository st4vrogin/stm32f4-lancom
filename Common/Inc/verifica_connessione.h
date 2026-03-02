/*****************************************************************
*
*	Project: VoIP SRC Terminal Server
*
*	Module: verifica_connessione.c
*	Created: 2015/09/01
*	Descr:		Verifica connessione
*
******************************************************************/

#ifndef __VERIFICA_CONNESSIONE_H__
#define __VERIFICA_CONNESSIONE_H__

/**************************************
	Definizione stati automa
***************************************/
#define	ST_VF_INIT         	0
#define	ST_VF_WAIT         	1
#define	ST_VF_EXEC_PING    	2
#define	ST_VF_VERIFICA_PING	3
#define	ST_VF_SCAMBIA      	4
#define	ST_VF_WAIT_SCAMBIA 	5
#define	ST_VF_RICONFIGURA_LAN 	6

#define	LAST_ST_VF				ST_VF_RICONFIGURA_LAN

/**************************************
	Definizioni varie
***************************************/

/**************************************
	Struttura dati
***************************************/

/**************************************
	Routines
***************************************/
void 	AutomaVerificaConnessione(void);

void 	AutoVF_ST_VF_INIT(void);
void 	AutoVF_ST_VF_WAIT(void);
void 	AutoVF_ST_VF_EXEC_PING(void);
void 	AutoVF_ST_VF_VERIFICA_PING(void);
void 	AutoVF_ST_VF_SCAMBIA(void);
void 	AutoVF_ST_VF_WAIT_SCAMBIA(void);
void 	AutoVF_ST_VF_RICONFIGURA_LAN(void);

#endif	/*  __VERIFICA_CONNESSIONE_H__ */

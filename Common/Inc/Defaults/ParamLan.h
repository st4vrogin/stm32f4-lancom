/*****************************************************************

	Project:

	Module:		ParamLan.h
	Created:
	Descr:		Valori predefiniti per i parametri di LAN (alto livello TCP/IP)

******************************************************************/

/*
typedef __packed struct
{
	uint16	key;

	uint8	usa_dhcp;			// 0 disattivo, 1 attivo
	uint8	ip_addr[4];			// Indirizzo lan da usare se (usa_dhcp = 0) oppure se dhcp non funzionante.
								//		I dati sono da considerarsi in quest'ordine (es.):
								//		adr_lan[0] = 192;
								//		adr_lan[1] = 168;
								//		adr_lan[2] = 1;
								//		adr_lan[3] = 100;
	uint8	netmask_addr[4];	// Come sopra
	uint8	gw_addr[4];			// Come sopra

	uint16	porta_ges_com[NUM_GES_COM_LAN];

} commonParametri_t;
*/

.key = 0x55AA,					//	uint16	key;
.usa_dhcp = 0,						//	uint8	usa_dhcp;			// 0 disattivo, 1 attivo

//{192,168,1,222},		//	uint8	ip_addr[4];			// Indirizzo lan da usare se (usa_dhcp = 0) oppure se dhcp non funzionante.
.ip_addr = {172,16,1,243},		//	uint8	ip_addr[4];			// Indirizzo lan da usare se (usa_dhcp = 0) oppure se dhcp non funzionante.

.netmask_addr = {255,255,255,0},		//	uint8	netmask_addr[4];	// Come sopra

//{192,168,1,1},			//	uint8	gw_addr[4];			// Come sopra
.gw_addr = {172,16,1,254},		//	uint8	gw_addr[4];			// Come sopra

.porta_ges_com = { 4000,					//	uint16	porta_ges_com_0;
4100,					//	uint16	porta_ges_com_1;
4200, }					//	uint16	porta_ges_com_2;

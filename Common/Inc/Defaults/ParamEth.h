/*****************************************************************

	Project: TERMINAL SERVER

	Module:		ParamEth.h
	Created:
	Descr:		Valori predefiniti per i parametri di basso livello Ethernet

******************************************************************/

/*
typedef __packed struct
{
	uint16	key;

	uint8	mac_scritto;		// Se = 0x00 -> non è ancora stato sostituito da quello di funzionamento
	uint8	mac_address[6];
} ethParametri_t;
*/

VALID_W16,					//	uint16	key;

0,						// uint8	mac_scritto,

{2,	0,	0,	0,	0,	0},	//	uint8	mac_address[6];

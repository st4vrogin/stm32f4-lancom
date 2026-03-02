/** ****************************************************************************
	@prog		Terminal server

	@file		MasterParametri.h
	@brief		Variabili di master

	@author		Siel Tre srl
	@version	2.0.0 2023-07-03
******************************************************************************/
= {
.key = 0x55AA,		// uint16	key;
.cfg_uart = {
	// Configurazione porte seriali
	{ .uart_speed = 115200, .n_bit = 8, .parity = 'n', .nbit_stop = 1	}, // Poseidon
	{ .uart_speed = 115200, .n_bit = 8, .parity = 'n', .nbit_stop = 1	}, // Poseidon
	{ .uart_speed = 115200, .n_bit = 8, .parity = 'n', .nbit_stop = 1	}, // Poseidon
	{ .uart_speed = 38400, .n_bit = 8, .parity = 'n', .nbit_stop = 1	}, // AIS
	{ .uart_speed = 2400, .n_bit = 8, .parity = 'e', .nbit_stop = 1	}, // OMC
	{ .uart_speed = 2400, .n_bit = 8, .parity = 'e', .nbit_stop = 1	}, // OMC
	{ .uart_speed = 2400, .n_bit = 8, .parity = 'e', .nbit_stop = 1	}, // OMC
	{ .uart_speed = 2400, .n_bit = 8, .parity = 'e', .nbit_stop = 1	}, // OMC
	{ .uart_speed = 115200, .n_bit = 8, .parity = 'n', .nbit_stop = 1	}, // DSP
	{ .uart_speed = 115200, .n_bit = 8, .parity = 'n', .nbit_stop = 1	}, // VoIP
},

.param_lan_com = {
	// Configurazione LAN - COM

	//LAN_COM_0
	{
		.porta_servizio = 		4001,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x03,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				0,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},

	//LAN_COM_1
	{
		.porta_servizio = 		4002,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x03,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				128,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},

	//LAN_COM_2
	{
		.porta_servizio = 		4003,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x03,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				128,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},

	//LAN_COM_3
	{
		.porta_servizio = 		4004,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x03,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				128,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},

	//LAN_COM_4
	{
		.porta_servizio = 		4005,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x03,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				128,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},

	//LAN_COM_5
	{
		.porta_servizio = 		4006,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x03,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				128,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},

	//LAN_COM_6
	{
		.porta_servizio = 		4007,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x03,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				128,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},

	//LAN_COM_7
	{
		.porta_servizio = 		4008,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x03,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				128,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},

	//LAN_COM_8
	{
		.porta_servizio = 		4009,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x03,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				128,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},

	//LAN_COM_9
	{
		.porta_servizio = 		4010,			// uint16	porta_servizio;
		.usa_carattere_per_tx = 0x01,			//	uint8	usa_carattere_per_tx;
		.carattere_per_tx = 	0x0A,			//	uint8	carattere_per_tx;
		.timeout_send_tx =		10,				//	uint16	timeout_send_tx;		// Se non ricevo più dati da timeout_send_tx (passi 5 mS), inoltro il messaggio sulla lan
		.len_max = 				128,				//	uint16	len_max;				// Attivo solo se > 0. Se ho già ricevuto più di len_max caratteri,
		.write_timeout = 		0,				//  uint16  write_timeout			//Attivo se > 0. Superato il timeout la connessione viene liberata
	},
},

	.param_flags_1.byte = 0x00,				// bit di configurazione - vedi struttura
	.param_flags_2.byte = 0x00,				// bit di configurazione - vedi struttura

	.timeout_refresh_wd = 0x0000,				// timeout_refresh_wd -> 0 or 0xFFFF = default timeout
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// free
},
}

# Terminal Server



## Progetto

Il progetto è basato su GCC ARM.

La compilazione richiede l'installazione di GCC ARM Bare Metal e Make.

Il Makefile principale genera i file TSCube.elf, TSCube.hex, TSCube.bin e TSCube.map nella directory build/

La prima generazione del progetto è stata eseguita mediante STM32CubeMX, è incluso il file TSCube.ioc di generazione.

La struttura della memoria è personalizzata rispetto al default per una serie di ragioni:

    - Questo applicativo dipende da un loader che deve essere precaricato sul target e che parte sempre all'accensione e ad ogni reset.
    - Il loader parte dalle zone di flash di default.
    - C'è una parametrizzazione permamente di molte configurazioni che deve essere caricata alla partenza da FLASH.
    - Per ragioni storiche di utilizzo dei device in produzione alcuni settori di FLASH interna possono essere danneggiati.

Per queste ragioni viene utilizzato un file di configurazione per il linker personalizzato chiamato TERMINAL_SERVER.ld. Ad ogni rigenerazione del
progetto a partire dal file .ioc, il Makefile viene riscritto puntando al file .ld di default (STM32F427VITx_FLASH.ld)

## Note FreeRTOS

Il porting di FreeRTOS Plus TCP non è supportato nativamente dai tool di STM32 (es. STM32CubeMX) se non per i chip all'ultimo grido. Per la versione STM32F4 CMSIS di FreeRTOS è consigliato la versione 3.1.0 di FreeRTOS Plus TCP.

Un'importante nota a riguardo della configurazione Ethernet si può trovare in FreeRTOS-Plus-TCP/source/portable/NetworkInterface/STM32Fxx/readme.md

### Istruzioni per la generazione del progetto e la compilazione con FreeRTOS+TCP

L'inizializzazione della LAN non deve essere fatta dal codice generato da CubeMX, ma dai drivers forniti con FreeRTOS+TCP. Per fare questo è necessario:

* Comunque abilitare la generazione del codice relativo alla periferica ETH in STM32CubeMX
* Generare il progetto con STM32CubeMX
* Rinominare o eliminare i file Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_eth.h e Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c
* Verificare che nel Makefile non venga compilato Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c ma Middlewares/Third_Party/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/STM32Fxx/stm32f4xx_hal_eth.c 
* Eliminare la routine MX_ETH_Init da main.c ed evitare che venga chiamata all'inizio del main
* Chiamare la routine FreeRTOSIPInit() che si occuperà di inizializzare sia la periferica che lo stack FreeRTOS

## Continuous integration

Il file .gitlab-ci.yml specifica quando eseguire il build e il versionamento. Si occupa di generare il file unico
da utilizzare con [TelecomConnect](https://gitlab.sieltre.local/radiocoast/winclients/TelecomConnect)

Gitlab CI genera anche un pacchetto .tar.gz contenente i file specificati al paragrafo precedente.

## Requirements e download

Per il download in produzione la scheda deve essere già precaricata con un [Loader](https://gitlab.sieltre.local/fwdev/loader_stm32f4) con versione >= 3.0.0

Per scaricare il FW è necessario utilizzare [TelecomConnect](https://gitlab.sieltre.local/radiocoast/winclients/TelecomConnect) con versione >= 1.5.1 in modalità file unico.


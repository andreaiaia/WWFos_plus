# WWFos_plus - un sistema operativo basato su PandOS+

Progetto per il corso di Sistemi Operativi, del Corso di Laurea Triennale in Informatica presso Alma Mater Studiorum, Università di Bologna.

## Introduzione

Il sistema operativo PandOS è l'ultimo di una serie di sistemi operativi didattici che prendono spunto dalle specifiche del sistema T.H.E. definito da Dijkstra nel 1968. L'ambiente usato per lo sviluppo del sistema è uMPS3, un emulatore per l'architettura MIPS fornito di interfaccia grafica e strumenti di Debug. Alcuni dei più noti prodotti basati su architettura MIPS sono: la Playstation 2, il Nintendo 64 e parti del software della Tesla Model S.

## Setup

Per poter compilare ed eseguire il codice è necessario trovarsi su un sistema operativo GNU/Linux e installare l'emulatore [uMPS3](https://github.com/virtualsquare/umps3).
A questo punto è sufficiente trovarsi nella directory phase2 e lanciare il comando `make all`, dopodiché per avviare la macchina virtuale basta seguire [questo tutorial](https://wiki.virtualsquare.org/#!education/tutorials/umps/getting_started.md#Booting_in_%C2%B5MPS).

## Fase 1

Di seguito alcune note riguardo dei comportamenti inaspettati che abbiamo riscontrato durante l'esecuzione dei test.

### Aggiunta in testa vs aggiunta in coda

Abbiamo notato che le funzioni `insertProcQ` (file **pcb.c**, riga **102**) e `insertBlocked` (file **asl.c**, riga **33**) passano correttamente i test **solamente** se usiamo la macro `list_add_tail`, mentre inserendo in testa con la macro `list_add` i test falliscono su entrambe le funzioni.

## Fase 2

In questa fase viene sviluppato il Kernel del nostro sistema operativo, in particolare questo livello si occupa di inizializzare il sistema, gestire lo scheduling dei processi e le eccezioni che possono avvenire durante l'esecuzione.
Di seguito ci soffermeremo su alcune scelte implementative che abbiamo dovuto prendere durante lo sviluppo del codice di questa fase.

### L'array dei semafori

L'array di interi inizializzato nel file main contiene 49 semafori (uno per ogni dispositivo, ad eccezione dei terminali che hanno due semafori ciascuno, e infine un semaforo per lo pseudo-clock).
Abbiamo scelto di dare il seguente ordine ai semafori dell'array:

- I primi 8 semafori (indici **0-7**) sono associati ai **Disk** devices;
- I successivi 8 semafori (indici **8-15**) sono associati ai **Tape** Devices;
- I successivi 8 semafori (indici **16-23**) sono associati ai **Network** Devices;
- I successivi 8 semafori (indici **24-31**) sono associati ai **Printer** Devices;
- I restanti 16 semafori (indici **32-47**) sono associati ai **Terminal** Devices nel seguente modo,
  - I semafori sono associati a due a due ai rispettivi terminali (quindi semaforo 32 e 33 sono associati al primo terminale, ad esempio), in questo modo i semafori _recv_ sono quelli di indice _pari_, mentre i semafori _transm_ sono quelli di indice _dispari_;
- Infine c'è il semaforo dello **pseudo-clock** all'indice **48**.

### SYSCALL.c

#### Implementazione della Terminate_Process

Per implementare la SYSCALL NSYS2 TERMINATEPROCESS abbiamo scelto di seguire un approccio ricorsivo, pur consapevoli della penalizzazione delle prestazioni che questo comporta rispetto ad un approccio puramente iterativo, perché abbiamo preferito mantenere la chiarezza del codice (essendo dopotutto questo un progetto a scopo didattico).
Per implementare questo approccio ricorsivo abbiamo fatto uso di due funzioni di appoggio (i.e. `void Exterminate(pcb_PTR)` e `pcb_PTR find_process(int)`) che, per ragioni di pulizia e leggibilità del codice, abbiamo preferito spostare in un file dedicato.

#### Implementazione della Do_IO_Device

Per la gestione della SYSCALL NSYS5, ovvero della DOIO, abbiamo cercato di ottimizzare le performance evitando dei cicli per cercare il semaforo desiderato.
Per comodità è stata definita una Macro nell'header file che, preso come parametro il puntatore commandAddr esegue un calcolo aritmetico per individuare il dispositivo in cui si trova il commandAddr fornito.
Nel definire questa Macro la prima volta è emerso subito il problema che essa non riusciva a distinguere, nel caso il commandAddr fosse contenuto in un dispositivo terminale, se questo fosse associato ad un semaforo recv o ad uno transm, con la conseguenza che la macro avrebbe sempre restituito un indice massimo di 40, ignorando ben 8 semafori.
Il modo più semplice per risolvere questo comportamento è stato modificare la macro in modo che dividesse per la metà della dimensione di un blocco di Registro dispositivo, in questo modo trova correttamente il semaforo da bloccare sul terminale, ma chiaramente questa modifica sballa i conti con gli indici. Questo errore viene risolto con un costrutto if-then-else che, nel caso l'indice trovato sia superiore a 63 (quindi avendo raddoppiato tutti equivarrebbe ad aver superato il 32esimo dispositivo), lo normalizza togliendo l'offset artificiosamente introdotto, in caso contrario per normalizzare basta dimezzare l'indice trovato e si ha il corretto indice di semaforo su cui eseguire la Passeren.

### Implementazione dell'Interrupt Handler

L'interrupt handler è stato gestito in due fasi:

- 1 -> trovare la "line" nel registro Cause.IP
- 2 -> richiamare la funzione appropriata in base alla line con un interrupt pending (partendo dall'indice piu' basso)

Le funzioni sono fedeli a quanto indicato dal manuale, un accorgimento utile è stato quello di usare una variabile "mask" che partisse da uno e raddoppiasse di volta in volta per fare degli AND bit a bit.

Abbiamo scelto di mettere in conto al processo corrente il tempo speso per gli interrupt, in particolare ciò che accade è:

- processo corrente è interrotto
- viene gestito l'interrupt
- interrupt ritorna controllo a processo corrente
- il processo corrente nel calcolo del tempo dedicatogli include quello speso per l'interrupt

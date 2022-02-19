# WWFos_plus - un sistema operativo basato su PandOS+
Progetto per il corso di Sistemi Operativi, del Corso di Laurea Triennale in Informatica presso Alma Mater Studiorum, Università di Bologna.

## Introduzione
Il sistema operativo PandOS è l'ultimo di una serie di sistemi operativi didattici che prendono spunto dalle specifiche del sistema T.H.E. definito da Dijkstra nel 1968. L'ambiente usato per lo sviluppo del sistema è uMPS3, un emulatore per l'architettura MIPS fornito di interfaccia grafica e strumenti di Debug. Alcuni dei più noti prodotti basati su architettura MIPS sono: la Playstation 2, il Nintendo 64 e parti del software della Tesla Model S.

## Setup
Per poter compilare ed eseguire il codice è necessario trovarsi su un sistema operativo GNU/Linux e installare l'emulatore [uMPS3](https://github.com/virtualsquare/umps3).
A questo punto è sufficiente trovarsi nella directory phase1 e lanciare il comando `make all`, dopodiché per avviare la macchina virtuale basta seguire [questo tutorial](https://wiki.virtualsquare.org/#!education/tutorials/umps/getting_started.md#Booting_in_%C2%B5MPS).

## Fase 1
Di seguito alcune note riguardo dei comportamenti inaspettati che abbiamo riscontrato durante l'esecuzione dei test.

### Aggiunta in testa vs aggiunta in coda
Abbiamo notato che le funzioni `insertProcQ` (file **pcb.c**, riga **102**) e `insertBlocked` (file **asl.c**, riga **33**) passano correttamente i test **solamente** se usiamo la macro `list_add_tail`, mentre inserendo in testa con la macro `list_add` i test falliscono su entrambe le funzioni. 

// Questo file implementa il test e esporta le
// variabili globali del livello support (sezioni 4.9 e 4.10)

#include "initProc.h"

#include "../h/pandos_types.h"
#include "../h/pandos_const.h"

// Strutture di supporto dei processi
support_t support_table[UPROCMAX];
list_head support_free;

// Semafori per tutti i dispositivi
int device_sem[DEVSEM_NUM];

int mainSemaphore;

void test_fase3()
{
    // Inizializzo le strutture dati della Swap Pool Table
    initSwapStructs();

    // Inizializzo il semaforo principale
    mainSemaphore = 0;

    // Fine del test, chiudiamo baracca e burattini
    HALT();


}

/**************************/
/*   PARCO GIOCHI ALEX    */
/**************************/


// semafori periferiche
int printer_sem[UPROCMAX];
int flash_sem[UPROCMAX];
int term_w_sem[UPROCMAX];
int term_r_sem[UPROCMAX];

void test_alex() {

    // inizializza swap pool table e swap semaphore
    initSwapStructs();
    
    // mettere a 1 semafori dei dispositivi I/O
    for(int i = 0; i < UPROCMAX; i++){
        printer_sem[i]    = 1;
        flash_sem[i]      = 1;
        term_w_sem[i] = 1;
        term_r_sem[i]  = 1;
    }

    // lanciare gli U-procs
    for(int i = 0; i < UPROCMAX; i++){
        
    }

}

//! lasciate ogne speranza o voi che leggete (da qui in giu' codice Alex capitolo 4.9)

/*
    inizializzare:
*        - swap pool table
*        - swap pool semaphore
*        - semafori delle periferiche I/O tutti a 1


    lanciare U-procs da 1 a 8:
        - settare i parametri della NSYS1:
            - stateprocess di ogni U-proc deve avere:
                PC = 0x8000.00B0
                SP = 0xC000.0000
                Status = usermode (con interrupt enabled e LocalTimer enabled)
                EntryHi.ASID = process's unique ID (integer 1..8)
!               ogni U-proc deve avere un ASID != 0 e che sia unico
            - supportstruct vanno inizializzate così:
?                 solo sup_asid, sup_exceptContext e privatePgTbl devono essere inizializzati PRIMA di chiamare la NSYS1
                - array di 8 supportStruct in test (possibile usare ASID come index array)
                - ogni supportStruct deve avere:
                    - sup_asid = process ASID
                    - sup_ExceptState[2] = i due stati del processore (state_t) che si trovano dove il Kernel li ha salvati al momento dell'eccezione  (eccezione del passaggio della gestione al livello di supporto)
                    - sup_exceptContext[2] = i 2 contesti del processore (context_t)
                       ogni contesto è una tripla (PC,SP,Status) e questi sono i due contesti usati dal Kernel per passare la gestione dell'eccezione al lvl supporto
                       - come inizializzare le context areas dle processore:
                            - PC fields:  0 (PGFAULTEXCEPT) deve essere settato sull'indirizzo del Support LVL TLB handler
                                          1 (GENERALEXCEPT) deve essere settato all'indirizzodel Support Lvl genExcHandler
                            - Status:     entrambi settati a kernel-mode con interrupt e PLT enabled
                            - SP:         entrambi i fields devono usare gli spazi stack allocati nella struttura di supporto
                                          Stacks grow “down” so set the SP fields to the address of the end of these areas. e.g. ... = &(...sup stackGen[499])
                    - sup_privatePgTbl[32] = la Page Table del processo
                    - sup_stackTLB[500] = l'area per lo stack del lvl di supporto (500integer = 2Kb)
                    - sup_stackGen[500] = area stack per il genExcHandler del lvl supporto


        - eseguire NSYS1 (vedi note in fondo)
            ciclo for su array delle support struct


    uno tra i due:
        - terminare tutti i figli degli U-procs una volta che hanno concluso (dovrò avere il process count a 0 così Kernel triggera HALT)
        - P  (NSYS3) su un semaforo a parte inizializzato a 0; in questo caso dopo che i figli degli U-procs hanno concluso si verificherà dedlock e kernel PANIC



?   NOTE   ?
    NSYS1  ->  SYSCALL(CREATEPROCESS, state_t *stateprocess, int prio, support_t *supportstructprocess)


*/
// Questo file implementa il test e esporta le
// variabili globali del livello support (sezioni 4.9 e 4.10)

#include "initProc.h"


// Strutture di supporto dei processi
support_t support_table[UPROCMAX];
struct list_head support_free;

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

state_t cpu_state_table[UPROCMAX]; // array di processor state per gli U-procs
//? non sono sicuro serva un array di stati processore per gli U-proc, forse basta crearne uno e 
//? spammarlo sull'inizializzazione di tutti gli U-proc?
//! Puoi usare la copy_state della phase2 e modificarla inzializzando tutte le cose a 0. 
//! Modificando anche i parametri con 1 solo, non hai bisogno di origin e state.

void test_alex()
{
    // inizializza swap pool table e swap semaphore
    initSwapStructs();
    mainSemaphore=0; // Richiesta da andrea
    // inizializza semafori device
    for(int i = 0; i < UPROCMAX; i++){
        printer_sem[i]    = 1;
        flash_sem[i]      = 1;
        term_w_sem[i]     = 1;
        term_r_sem[i]     = 1;
    }

    // inizializzazione stato processore degli U-proc
    for (int i = 0; i < UPROCMAX; i++){
        cpu_state_table[i].pc_epc = UPROCSTARTADDR;
        cpu_state_table[i].gpr[24] = UPROCSTARTADDR;   //gpr[24] corrisponde a reg_t9
        cpu_state_table[i].entry_hi = i+1;
        cpu_state_table[i].status = STATUS_TE | STATUS_IM(IL_TIMER) | STATUS_IEp | STATUS_IM_MASK | STATUS_KUc; // user mode con tutti interrupt e local timer enabled;
        //todo quale minchia è lo stack pointer nello state_t? sai, dovrei inizializzarlo 
        //! processo poi hai una struttura state_t che avrà il campo p_s che avrà il campo reg_sp
        //! riga 65 modificare gpr[24] non sei Conte, porcoddio (GDPR)
    }

    // inizializzazione support struct degli U-proc
    for(int i = 0; i < UPROCMAX; i++){
        support_table[i].sup_asid = i + 1; //  ogni U-proc deve avere un ASID != 0 unico
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].pc = (memaddr)TLB_ExcHandler; 
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].status = STATUS_TE | STATUS_IM(IL_TIMER) | STATUS_IEp | STATUS_IM_MASK | STATUS_KUp ^ STATUS_KUp;
        //todo capire cosa fanno tutte quelle costanti su status
        //! Guarda main phase2 riga 78 circa, guarda inizio capitolo 3 
            // STATUS_TE, STATUS_IM(IL_TIMER)
            // STATUS_IEp  dovrebbe essere InterruptEnabled process
            // STATUS_IM_MASK  dovrebbe attivare tutte le interrupt line (non so bene cosa voglia dire nemmeno io)
            // STATUS_KUp   potrebbe essere KernelUser process o qualcosa del genere
            // lo ^ è lo xor
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].stackPtr = &(...sup_stackGen[499]); //? da capire cosa e come e perché stackGen[499]
        //todo da capire come funziona il discorso del sup_StackGen[500]
        support_table[i].sup_exceptContext[GENERALEXCEPT].pc = (memaddr)generalExcHandler;
        support_table[i].sup_exceptContext[GENERALEXCEPT].status = STATUS_TE | STATUS_IM(IL_TIMER) | STATUS_IEp | STATUS_IM_MASK | STATUS_KUp ^ STATUS_KUp;
        support_table[i].sup_exceptContext[GENERALEXCEPT].stackPtr = &(...sup_stackGen[499]);

        //todo capire come inizializzare la privatePgTbl
        support_table[i].sup_privatePgTbl ;
    }


    // lanciare gli U-procs
    for (int i = 0; i < UPROCMAX; i++){
        SYSCALL(CREATEPROCESS, &(cpu_state_table[i]), PROCESS_PRIO_LOW, support_table[i]);
    }


    // aspettare che terminino tutti i processi
    //? codice preso da Frau con lo scopo di chiedere spiegazioni
    
    for (int i = 0; i < UPROCMAX; ++i)
        SYSCALL(PASSEREN, (int)&mainSemaphore, 0, 0);
    SYSCALL(TERMPROCESS, 0, 0, 0);
    //todo qui a uncerto punto i processi vengono conclusi e bisogna spegnere la baracca non ho idea per ora
    //! Andrea utilizzerebbe l'approccio terminate, non quello verhogen e poi ci guardiamo.
}

//! lasciate ogne speranza o voi che programmate (da qui in giu' codice Alex capitolo 4.9)

/*
*   inizializzare:
*        - swap pool table
*        - swap pool semaphore
*        - semafori delle periferiche I/O tutti a 1


*    lanciare U-procs da 1 a 8:
*        - settare i parametri della NSYS1:
*            - stateprocess di ogni U-proc deve avere:
*                PC = 0x8000.00B0
*                SP = 0xC000.0000
*                Status = usermode (con interrupt enabled e LocalTimer enabled)
*                EntryHi.ASID = process's unique ID (integer 1..8)
*            - supportstruct vanno inizializzate così:
!                 solo sup_asid, sup_exceptContext e privatePgTbl devono essere inizializzati PRIMA di chiamare la NSYS1
*                - array di 8 supportStruct in test (possibile usare ASID come index array)
*                - ogni supportStruct deve avere:
*                    - sup_asid = process ASID
*                    - sup_ExceptState[2] = i due stati del processore (state_t) che si trovano dove il Kernel li ha salvati al momento dell'eccezione  (eccezione del passaggio della gestione al livello di supporto)
*                    - sup_exceptContext[2] = i 2 contesti del processore (context_t)
*                        ogni contesto è una tripla (PC,SP,Status) e questi sono i due contesti usati dal Kernel per passare la gestione dell'eccezione al lvl supporto
*                        - come inizializzare le context areas dle processore:
*                            - PC fields:  0 (PGFAULTEXCEPT) deve essere settato sull'indirizzo del Support LVL TLB handler;  1 (GENERALEXCEPT) deve essere settato all'indirizzodel Support Lvl genExcHandler
*                            - Status:     entrambi settati a kernel-mode con interrupt e PLT enabled
*                            - SP:         entrambi i fields devono usare gli spazi stack allocati nella struttura di supporto
                    - sup_privatePgTbl[32] = la Page Table del processo
                    - sup_stackTLB[500] = l'area per lo stack del lvl di supporto (500integer = 2Kb)
                    - sup_stackGen[500] = area stack per il genExcHandler del lvl supporto
                    ? non ho capito bene il discorso del stackGen[500], devo crearlo io? idem per lo stackTLB; in ogni caso ne va creato uno per processo?


*    eseguire NSYS1 (vedi note in fondo) (ciclo for su array delle support struct)

    uno tra i due:
        - terminare tutti i figli degli U-procs una volta che hanno concluso (dovrò avere il process count a 0 così Kernel triggera HALT)
        - P  (NSYS3) su un semaforo a parte inizializzato a 0; in questo caso dopo che i figli degli U-procs hanno concluso si verificherà dedlock e kernel PANIC
*/
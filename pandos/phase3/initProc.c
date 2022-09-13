// Questo file implementa il test e esporta le
// variabili globali del livello support (sezioni 4.9 e 4.10)

#include "initProc.h"

// Strutture di supporto dei processi
support_t support_table[UPROCMAX];
struct list_head support_free;

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

//? per mettere processi in user mode ho aggiunto | STATUS_KUc ma non sono certo sia corretto
    //? RISPOSTA: no, non credo vada messo, in quanto il bit dev'essere a 0 perché il kernel mode sia attivo (pag. 9 pops)
    //? di conseguenza non ci metto nulla così dovrebbe essere a 0 (questo sulla support_struct)
    //? mentre nel processor_state c'era scritto che dev'essere in user mode e quindi ho aggiunto STATUS_KUc

//? se per le struct serve l'array non credo sia lo stesso per il processor_state visto che nella struct c'é la variabile in carne ed ossa e non un ptr

support_t support_table[UPROCMAX];  // array delle strutture di supporto dei processi

void test_alex()
{
    state_t stateaux;

    mainSemaphore = 0; // Richiesta da andrea

    // inizializza swap pool table e swap semaphore
    initSwapStructs();

    // inizializza semafori device
    for (int i = 0; i < UPROCMAX; i++){
        printer_sem[i] = 1;
        flash_sem[i] = 1;
        term_w_sem[i] = 1;
        term_r_sem[i] = 1;
    }

    // inizializzazione (creazione lista) degli 8 U-proc
    for (int i = 0; i < UPROCMAX; i++){

        // inizializzazione processor_state
        stateaux.pc_epc = UPROCSTARTADDR;
        stateaux.reg_t9 = UPROCSTARTADDR;
        stateaux.reg_sp = USERSTACKTOP;
        stateaux.status = ALLOFF | IEPON | IMON | TEBITON | STATUS_KUc; // main phase2 riga 68: interrupt e PLTimer abilitati
        stateaux.entry_hi = i + 1;   // ASID != 0

        // inizializzazione support_struct
        support_table[i].sup_asid = i + 1;   // ASID != 0
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].pc = (memaddr)TLB_ExcHandler;
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].status = ALLOFF | IEPON | IMON | TEBITON;
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].stackPtr = &(support_table[i].sup_stackTLB[499]);
        support_table[i].sup_exceptContext[GENERALEXCEPT].pc = (memaddr)generalExcHandler;
        support_table[i].sup_exceptContext[GENERALEXCEPT].status = ALLOFF | IEPON | IMON | TEBITON;
        support_table[i].sup_exceptContext[GENERALEXCEPT].stackPtr = &(support_table[i].sup_stackGen[499]);

        // creazione processo
        SYSCALL(CREATEPROCESS, &(stateaux), PROCESS_PRIO_LOW, &(support_table[i]));
    }

    // aspettare che terminino tutti i processi
    // todo capire come terminare i processi
    for (int i = 0; i < UPROCMAX; ++i){
        SYSCALL(PASSEREN, (int)&mainSemaphore, 0, 0);
    }
    SYSCALL(TERMPROCESS, 0, 0, 0);
}

//* Legenda delle macro (alcune a pag.9 pops)
/* 
    STATUS_TE, STATUS_IM(IL_TIMER)
    STATUS_IEp  dovrebbe essere InterruptEnabled process
    STATUS_IM_MASK  dovrebbe attivare tutte le interrupt line (non so bene cosa voglia dire nemmeno io)
    STATUS_KUp   potrebbe essere KernelUser process o qualcosa del genere
    lo ^ è lo xor
    ALLOFF = serie di 0
    IEPON = Interrupt Enable Previous ON
    IMON = Interrupt Mask ON
    TEBITON = Time Enable BIT ON
*/

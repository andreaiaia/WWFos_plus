// Questo file implementa il test e esporta le
// variabili globali del livello support (sezioni 4.9 e 4.10)

#include "initProc.h"

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

void test_alex()
{
    // Inizializzo le strutture dati della Swap Pool Table
    initSwapStructs();

    // Inizializzo il semaforo principale
    mainSemaphore = 0;

    // Inizializzo i semafori dei dispositivi a 1
    for (int i = 0; i < UPROCMAX; i++)
    {
        printer_sem[i] = 1;
        flash_sem[i] = 1;
        term_w_sem[i] = 1;
        term_r_sem[i] = 1;
    }

    // Creo lo stato predefinito dei processi
    state_t initial_status;
    initial_status.pc_epc = UPROCSTARTADDR;
    initial_status.reg_t9 = UPROCSTARTADDR;
    initial_status.reg_sp = USERSTACKTOP;
    initial_status.status = ALLOFF | IEPON | IMON | TEBITON | USERPON;

    // inizializzazione (creazione lista) degli 8 U-proc
    for (int i = 0; i < UPROCMAX; i++)
    {
        // Assegno ad ogni processo il suo ASID
        initial_status.entry_hi = (i + 1) << ASIDSHIFT;

        // Replico l'assegnamento dell'ASID ma nella support struct
        support_table[i].sup_asid = i + 1;
        // Imposto l'exceptContext per quando si verifica un page fault
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].pc = (memaddr)TLB_ExcHandler;
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].status = ALLOFF | IEPON | IMON | TEBITON;
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].stackPtr = &(support_table[i].sup_stackTLB[499]);
        // Imposto l'exceptContext per quando si verifica una eccezione qualsiasi
        support_table[i].sup_exceptContext[GENERALEXCEPT].pc = (memaddr)generalExcHandler;
        support_table[i].sup_exceptContext[GENERALEXCEPT].status = ALLOFF | IEPON | IMON | TEBITON;
        support_table[i].sup_exceptContext[GENERALEXCEPT].stackPtr = &(support_table[i].sup_stackGen[499]);

        // Infine creo il processo
        SYSCALL(CREATEPROCESS, &(initial_status), PROCESS_PRIO_LOW, &(support_table[i]));
    }

    for (int i = 0; i < UPROCMAX; ++i)
    {
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

// Questo file implementa il test e esporta le
// variabili globali del livello support (sezioni 4.9 e 4.10)

#include "initProc.h"

void test_fase3()
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
        support_table[i].sup_exceptContext[PGFAULTEXCEPT].stackPtr = support_table[i].sup_stackTLB[499]; //rimosso & perché è unsigned int
        // Imposto l'exceptContext per quando si verifica una eccezione qualsiasi
        support_table[i].sup_exceptContext[GENERALEXCEPT].pc = (memaddr)generalExcHandler;
        support_table[i].sup_exceptContext[GENERALEXCEPT].status = ALLOFF | IEPON | IMON | TEBITON;
        support_table[i].sup_exceptContext[GENERALEXCEPT].stackPtr = support_table[i].sup_stackGen[499]; //rimosso & perché è un unsigned int

        // Infine creo il processo
        SYSCALL(CREATEPROCESS, &(initial_status), PROCESS_PRIO_LOW, &(support_table[i]));
    }

    for (int i = 0; i < UPROCMAX; ++i)
    {
        SYSCALL(PASSEREN, (int)&mainSemaphore, 0, 0);
    }

    // Finito, chiudiamo baracca e burattini
    SYSCALL(TERMPROCESS, 0, 0, 0);
}

//* Legenda delle macro (alcune a pag.9 pops)
/*
    ALLOFF = serie di 0
    IEPON = Interrupt Enable Previous ON
    IMON = Interrupt Mask ON
    TEBITON = Time Enable BIT ON
*/

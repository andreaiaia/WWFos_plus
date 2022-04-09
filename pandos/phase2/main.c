#include "scheduler.h" 
#include "exception.h"
#include "interrupt.h"

passupvector_t *pu_vector;

//* Dichiarazioni di funzioni esterne */
// Funzione fornita dalle specifiche, la riscriveremo nella prossima fase
extern void uTLB_RefillHandler();
// La funzione test si trova nel file di test fornito
extern void test();


//* A LONG TIME AGO, IN A MAIN FUNCTION FAR FAR AWAY */
int main()
{
    // Inizializzo le variabili globali e la coda dei processi
    initPcbs();
    initASL();
    proc_count = 0;
    soft_count = 0;
    mkEmptyProcQ(high_ready_q);
    mkEmptyProcQ(low_ready_q);
    current_p = NULL;
    for (int i = 0; i < DEVSEM_NUM; i++)
    {
        device_sem[i] = 0;
    }

    // Inizializzo il Passup Vector
    pu_vector = (passupvector_t *)PASSUPVECTOR;
    // Popolo il Passup Vector
    pu_vector->tlb_refill_handler = (memaddr)uTLB_RefillHandler;
    pu_vector->tlb_refill_stackPtr = KERNELSTACK;

    pu_vector->exception_handler = (memaddr)exceptionHandler;
    pu_vector->exception_stackPtr = KERNELSTACK;

    /**
     * Inizializzo il system-wide clock a 100ms usando la macro
     * fornita in const.h (che prende argomenti in microsecondi)
     * Come argomento uso la costante PSECOND (100000 microsec = 100 millisec)
     * Moltiplicando per TIMESCALEADDR mi assicuro che il timer sia tarato con le
     * impostazioni dell'emulatore di umps
     */
    LDIT(PSECOND * *((memaddr *)TIMESCALEADDR));

    // Creo un processo (a bassa priorità) da inserire nella Ready queue
    pcb_PTR kernel_mode_proc = allocPcb();

    /**
     * Imposto lo state_t su kernel mode, interrupt abilitati e Processor Local Time abilitato.
     * Uso l'or | per sommare i bit del registro e accenderli dove serve con le macro da pandos_const.h
     * ALLOFF = serie di 0
     * IEPON = Interrupt Enable Previous ON
     * IMON = Interrupt Mask ON
     * TEBITON = Time Enable BIT ON
     * Bisogna usare i Previous bit invece dei current quando si inizializza
     * un nuovo processor state
     */
    kernel_mode_proc->p_s.status = IEPON | IMON | TEBITON;

    // Imposto lo Stack Pointer su RAMTOP
    /**
     * Lo Stack Pointer è la 26esima entry del gpr, che grazie
     * alle macro definite in types.h può essere richiamata
     * semplicemente con notazione puntata sotto al nome di reg_sp
     */
    RAMTOP(kernel_mode_proc->p_s.reg_sp);
    /**
     * Come indicato sul manuale, per ragioni tecniche va
     * inizializzato allo steso modo anche il registro t9 del gpr
     */
    RAMTOP(kernel_mode_proc->p_s.reg_t9);

    // Imposto il PC sull'indirizzo della funzione test
    kernel_mode_proc->p_s.pc_epc = (memaddr)test;

    // ? imposta p_prio a 0

    // Finalmente inserisco il processo impostato nella ready queue
    insertProcQ(low_ready_q, kernel_mode_proc);
    proc_count++;

    // Chiamo lo Scheduler
    scheduler();

    return 0;
}

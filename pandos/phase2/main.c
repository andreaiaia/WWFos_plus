#include "scheduler.h"
#include "exception.h"
#include "interrupt.h"

//* Variabili Globali */
// Process Count - Contatore processi vivi (started but not yet finished)
int proc_count;
// Soft-Block Count - Contatore dei processi vivi ma bloccati
int soft_count;
// Queue dei processi ad alta priorità
struct list_head high_ready_q;
// Queue dei processi a bassa priorità
struct list_head low_ready_q;
// Array di tutti i processi creati
pcb_PTR all_processes[MAXPROC];
// Current Process
pcb_PTR current_p, yielded;
// Device Semaphores - we need 49 sem in total
int device_sem[DEVSEM_NUM];

cpu_t start;
cpu_t finish;

passupvector_t *pu_vector;

//* A LONG TIME AGO, IN A MAIN FUNCTION FAR FAR AWAY */
int main()
{
    // Inizializzo le variabili globali e le code dei processi
    initPcbs();
    initASL();
    proc_count = 0;
    soft_count = 0;
    mkEmptyProcQ(&high_ready_q);
    mkEmptyProcQ(&low_ready_q);
    current_p = NULL;
    yielded = NULL;
    for (int i = 0; i < DEVSEM_NUM; i++)
    {
        device_sem[i] = 0;
    }
    for (int j = 0; j < MAXPROC; j++)
    {
        all_processes[j] = NULL;
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
    LDIT(PSECOND);
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
    kernel_mode_proc->p_s.status = ALLOFF | IEPON | IMON | TEBITON;
    /**
     * Imposto lo Stack Pointer su RAMTOP.
     * Lo Stack Pointer è la 26esima entry del gpr, che grazie
     * alle macro definite in types.h può essere richiamata
     * semplicemente con notazione puntata sotto al nome di reg_sp
     */
    RAMTOP(kernel_mode_proc->p_s.reg_sp);
    // Imposto il PC sull'indirizzo della funzione test
    kernel_mode_proc->p_s.pc_epc = (memaddr)test_fase3;
    /**
     * Come indicato sul manuale, per ragioni tecniche va
     * inizializzato allo stesso modo anche il registro t9 del gpr
     */
    kernel_mode_proc->p_s.reg_t9 = (memaddr)test_fase3;
    // Essendo un processo a bassa priorità, imposto il campo prio di conseguenza
    kernel_mode_proc->p_prio = 0;
    // Finalmente inserisco il processo inizializzato nella ready queue
    insertProcQ(&low_ready_q, kernel_mode_proc);
    proc_count++;
    // Aggiorno il vettore dei processi
    all_processes[0] = kernel_mode_proc;
    // Chiamo lo Scheduler
    scheduler();

    return 0;
}

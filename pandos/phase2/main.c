#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"
#include "main.h"
#include "scheduler.h"

/* Costanti */
// Numero di semafori dei dispositivi
#define DEVSEM_NUM 49;

/* Variabili Globali */

// Process Count - Contatore processi vivi (started but not yet finished)
int proc_count;
// Soft-Block Count - Contatore dei processi avviati ma non ancora terminati (e quindi bloccati)
int soft_count;

// Le code dei processi ready sono dichiarate in scheduler.h
extern list_head *high_ready_q;
extern list_head *low_ready_q;

// Current Process - Puntatore a pcb in stato "Running" (correntemente attivo)
pcb_PTR current_p;
// Device Semaphores - we need 49 sem in total
int device_sem[DEVSEM_NUM];
// Passup Vector
passupvector_t *pu_vector;

/* Dichiarazioni di funzioni */

// Funzione fornita dalle specifiche, la riscriveremo nella prossima fase
void uTLB_RefillHandler()
{
    setENTRYHI(0x80000000);
    setENTRYLO(0x00000000);
    TLBWR();
    LDST((state_PTR)0x0FFFF000);
}
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
    // ! Dare nome appropriato al posto di fooBar
    pu_vector->exception_handler = (memaddr)fooBar;
    pu_vector->exception_stackPtr = KERNELSTACK;

    /**
     * Inizializzo il system-wide clock a 100ms usando la macro
     * fornita in const.h (che prende argomenti in microsecondi)
     * Come argomento uso la costante PSECOND (100000micros)
     */
    LDIT(PSECOND);

    // Creo un processo (a bassa priorità) da inserire nella Ready queue
    pcb_PTR kernel_mode_proc = allocPcb();

    /**
     * Imposto lo state_t su kernel mode, interrupt abilitati e Processor Local Time abilitato.
     * Uso l'or | per sommare i bit del registro e accenderli dove serve con le macro da pandos_const.h
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

    // Finalmente inserisco il processo impostato nella ready queue
    insertProcQ(low_ready_q, kernel_mode_proc);
    proc_count++;

    // Chiamo lo Scheduler
    scheduler();

    return 0;
}

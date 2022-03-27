#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/asl.c"
#include "scheduler.h"

/* Costanti */
#define SEM_NUM 49;

/* Variabili Globali */

// Process Count - Contatore processi vivi (started but not yet finished)
int proc_count;
// Soft-Block Count - Contatore dei processi avviati ma non ancora terminati (e quindi bloccati)
int soft_count;
// Ready Queue - Puntatore a Tail della coda dei pcb che sono in stato "Ready"
list_head *ready_q;
// Current Process - Puntatore a pcb in stato "Running" (correntemente attivo)
pcb_PTR current_p;
// Device Semaphores - we need 49 sem in total
int device_sem[SEM_NUM];
// Passup Vector
passupvector_t *pu_vector;

// Funzione fornita dalle specifiche, la riscriveremo nella prossima fase
void uTLB_RefillHandler()
{
    setENTRYHI(0x80000000);
    setENTRYLO(0x00000000);
    TLBWR();
    LDST((state_PTR)0x0FFFF000);
}

// TODO: Inizializzare il kernel
int main()
{
    // Inizializzo le variabili globali e inizializzo la coda dei processi
    initPcbs();
    initASL();
    proc_count = 0;
    soft_count = 0;
    mkEmptyProcQ(ready_q);
    current_p = NULL;
    for (int i = 0; i < SEM_NUM; i++)
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

    // Inizializzo il system-wide clock
    // Uso la macro fornita in const.h (che prende argomenti in microsecondi)
    LDIT(100000);

    // Creo un processo (a bassa priorità) da inserire nella Ready queue
    pcb_PTR kernel_mode_proc = allocPcb();

    // Inizializzo il processor state del pcb
    state_t proc_state;
    STST(&proc_state);
    /*
     * Imposto lo state su kernel mode, interrupt abilitati e Processor Local Time abilitato
     * Uso l'or | per sommare i bit del registro e accenderli dove serve con le macro da pandos_const.h
     * IECON = Interrupt enable current ON
     * IMON = Interrupt Mask ON
     * TEBITON = Time enable bit ON
     */
    proc_state.status = ALLOFF | IECON | IMON | TEBITON;

    // Imposto lo Stack Pointer su RAMTOP
    RAMTOP(gpr); // ! non sono sicuro di niente

    // Imposto il PC sull'indirizzo della funzione test
    proc_state.pc_epc = (memaddr)test;

    // Finalmente inserisco il processo impostato nella ready queue
    insertProcQ(ready_q, kernel_mode_proc);
    proc_count++;

    return 0;
}

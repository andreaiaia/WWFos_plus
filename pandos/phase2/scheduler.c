#include "scheduler.h"

void scheduler()
{
    // Se un processo è in corso
    if (current_p != NULL && current_p->p_semAdd == NULL)
    {
        // Leggo il time of day
        STCK(finish);
        // Aggiungo il tempo trascorso al tempo impiegato dal processo
        current_p->p_time = (current_p->p_time) + (finish - start);
        STCK(start);
        LDST(PROCESSOR_SAVED_STATE);
    }
    // Se la coda dei processi ad ALTA priorità è non-vuota
    else if (!emptyProcQ(&high_ready_q))
    {
        load_new_proc(&high_ready_q);
    }
    // Se la coda dei processi a BASSA priorità è non-vuota
    else if (!emptyProcQ(&low_ready_q))
    {
        if (yielded != NULL)
            insertProcQ(&high_ready_q, yielded);

        // Imposto il PLT su 5ms
        setTIMER(TIMESLICE);
        load_new_proc(&low_ready_q);
    }
    // Se le code sono entrambe vuote
    else
    {
        if (proc_count == 0)
            HALT(); // Spegne il computer
        else if (soft_count > 0)
        {
            // Imposto lo stato corrente per accettare interrupt
            // E disabilito il tutto il resto (quindi anche il PLT)
            unsigned int waitingStatus = (getSTATUS() | IECON | IEPON | IMON | TEBITON) ^ TEBITON;
            setSTATUS(waitingStatus);
            WAIT(); // Aspettando un interrupt
            scheduler();
        }
        else
        {
            // Siamo in deadlock
            PANIC();
        }
    }
}

// Funzione per prendere un processo dalla coda passata e avviarne l'esecuzione
void load_new_proc(struct list_head *queue)
{
    // Prendo il processo da avviare
    current_p = removeProcQ(queue);
    // Faccio partire il timer leggendo il Time of Day
    STCK(start);
    // Carico lo stato del processo sulla CPU
    LDST(&(current_p->p_s));
}
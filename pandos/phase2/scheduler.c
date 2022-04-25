#include "scheduler.h"

void scheduler()
{
    // Se la coda dei processi ad ALTA priorità è non-vuota
    if (!emptyProcQ(&high_ready_q))
        load_new_proc(&high_ready_q);
    // Se la coda dei processi a BASSA priorità è non-vuota
    else if (!emptyProcQ(&low_ready_q))
    {
        if (yielded != NULL)
        {
            insertProcQ(&high_ready_q, yielded);
            yielded = NULL;
        }
        // Imposto il PLT su 5ms
        setTIMER(TIMESLICE * *((memaddr *)TIMESCALEADDR));
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
            unsigned int waitingStatus = getSTATUS() | IECON | IMON;
            // Carico il PLT al massimo per evitare PLTTimerInterrupts
            setTIMER(MAX_TIME);
            current_p = NULL;
            setSTATUS(waitingStatus);
            // Entro in WAIT
            WAIT();
            /**
             * Mentre sono in wait continuo a controllare
             * se ci sono nuovi processi da lanciare
             */
            scheduler();
        }
        else if (soft_count == 0)
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
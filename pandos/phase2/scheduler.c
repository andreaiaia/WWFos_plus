#include "scheduler.h"

void scheduler()
{
    klog_print("SC1\n");
    // Se un processo è in corso
    if (current_p != NULL)
        klog_print("SC1\n");
    {
        // Leggo il time of day
        klog_print("SC2\n");
        STCK(finish);
        // Aggiungo il tempo trascorso al tempo impiegato dal processo
        klog_print("SC3\n");
        current_p->p_time = (current_p->p_time) + (finish - start);
    }
    klog_print("SC4\n");
    // Se la coda dei processi ad ALTA priorità è non-vuota
    if (!emptyProcQ(&high_ready_q))
    {
        klog_print("SC5\n");
        load_new_proc(&high_ready_q);
    }
    // Se la coda dei processi a BASSA priorità è non-vuota
    else if (!emptyProcQ(&low_ready_q))
    {
        klog_print("SC6\n");
        if (yielded != NULL)
            insertProcQ(&high_ready_q, yielded);

        klog_print("SC7\n");
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
            unsigned int waitingStatus = IECON | IMON;
            setSTATUS(waitingStatus);
            current_p = NULL;

            WAIT(); // Aspettando un interrupt
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
    // Imposto il PLT su 5ms
    setTIMER(TIMESLICE);
    // Prendo il processo da avviare
    current_p = removeProcQ(queue);
    // Faccio partire il timer leggendo il Time of Day
    STCK(start);
    // Carico lo stato del processo sulla CPU
    LDST(&(current_p->p_s));
}
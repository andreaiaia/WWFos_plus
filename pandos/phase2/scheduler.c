#include "scheduler.h"

void scheduler()
{
    klog_print("SC1\n");
    // Se un processo è in corso
    // if (current_p == NULL) klog_print("Current_p nullo\n");
    // if (current_p != NULL) klog_print("Current_p non nullo\n");
    if (current_p != NULL && current_p->p_semAdd == NULL)
    {
        // Leggo il time of day
        klog_print("SC2\n");
        STCK(finish);
        // Aggiungo il tempo trascorso al tempo impiegato dal processo
        klog_print("SC3\n");
        current_p->p_time = (current_p->p_time) + (finish - start);
        STCK(start);
        LDST(PROCESSOR_SAVED_STATE);
    }
    // Se la coda dei processi ad ALTA priorità è non-vuota
    else if (!emptyProcQ(&high_ready_q))
    {
        klog_print("SC5\n");
        load_new_proc(&high_ready_q);
    }
    // Se la coda dei processi a BASSA priorità è non-vuota
    else if (!emptyProcQ(&low_ready_q))
    {
        klog_print("SC6\n");
        if (yielded != NULL)
        {
            klog_print("SC6.1\n");
            insertProcQ(&high_ready_q, yielded);
        }
        // Imposto il PLT su 5ms
        setTIMER(TIMESLICE);
        klog_print("SC7\n");
        load_new_proc(&low_ready_q);
    }
    // Se le code sono entrambe vuote
    else
    {
        klog_print("SC8\n");
        if (proc_count == 0)
            HALT(); // Spegne il computer
        else if (soft_count > 0)
        {
            klog_print("SC9\n");
            // Imposto lo stato corrente per accettare interrupt
            // E disabilito il tutto il resto (quindi anche il PLT)
            unsigned int waitingStatus = (getSTATUS() | IECON | IEPON | IMON | TEBITON) ^ TEBITON;
            klog_print("SC10\n");
            setSTATUS(waitingStatus);
            klog_print("SC11\n");
            WAIT(); // Aspettando un interrupt
            scheduler();
        }
        else
        {
            klog_print("SC13\n");
            // Siamo in deadlock
            PANIC();
            klog_print("SC14\n");
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
    klog_print("LOAD\n");
    LDST(&(current_p->p_s));
}
#include "scheduler.h"

void scheduler()
{
    // setTIMER(0xFFFFFFFF); //!usata solo per debug
    klog_print("SH\n");
    if (!emptyProcQ(&high_ready_q))
    {
        klog_print("SH_HIGH\n");
        load_new_proc(&high_ready_q);
    }
    // Se la coda dei processi a BASSA priorità è non-vuota
    else if (!emptyProcQ(&low_ready_q))
    {
        if (yielded != NULL)
        {
            klog_print("SH_YIELDED\n");
            insertProcQ(&high_ready_q, yielded);
        }
        klog_print("SH_LOW\n");
        // Imposto il PLT su 5ms
        setTIMER(TIMESLICE);
        // setTIMER(MAX_TIME);
        load_new_proc(&low_ready_q);
    }
    // Se le code sono entrambe vuote
    else
    {
        klog_print("SH_BOTH_EMPTY\n");
        klog_print_hex(proc_count);
        klog_print("\n");
        klog_print_hex(soft_count);
        klog_print("\n");
        if (proc_count == 0)
            HALT(); // Spegne il computer
        else if (soft_count > 0)
        {
            klog_print("SH_SB\n");
            // Imposto lo stato corrente per accettare interrupt
            // E disabilito il tutto il resto (quindi anche il PLT)
            unsigned int waitingStatus = getSTATUS() | IECON | IEPON | IMON;
            klog_print("SH_SET_STATUS\n");
            setTIMER(MAX_TIME);
            setSTATUS(waitingStatus);
            klog_print("SH_WAIT\n");
            WAIT(); // Aspettando un interrupt
            scheduler();
        }
        else
        {
            klog_print("SH_GONNA_PANIK\n");
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
    klog_print("LOAD\n");
    LDST(&(current_p->p_s));
}
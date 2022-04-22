#include "scheduler.h"

void scheduler()
{
    // setTIMER(0xFFFFFFFF); //!usata solo per debug
    klog_print("SH - entro\n");
    /*if (current_p != NULL && current_p->p_semAdd == NULL)
    {
        // Leggo il time of day
        klog_print("SH_RUNNING\n");
        STCK(finish);
        // Aggiungo il tempo trascorso al tempo impiegato dal processo
        current_p->p_time = (current_p->p_time) + (finish - start);
        STCK(start);
        LDST(PROCESSOR_SAVED_STATE);
        klog_print("SH_se_mi_leggi_e_un_problema\n");
    }
    // Se la coda dei processi ad ALTA priorità è non-vuota
    else*/
    if (!emptyProcQ(&high_ready_q))
    {
        klog_print("SH_HIGH - ce proc in high q\n");
        load_new_proc(&high_ready_q);
    }
    // Se la coda dei processi a BASSA priorità è non-vuota
    else if (!emptyProcQ(&low_ready_q))
    {
        if (yielded != NULL)
        {
            klog_print("SH yielded != NULL\n");
            insertProcQ(&high_ready_q, yielded);
        }
        klog_print("SH - ce proc in low q\n");
        // Imposto il PLT su 5ms
        setTIMER(TIMESLICE);
        // setTIMER(MAX_TIME);
        load_new_proc(&low_ready_q);
    }
    // Se le code sono entrambe vuote
    else
    {
        klog_print("SH - code vuote\n");
        klog_print_hex(proc_count);
        klog_print("\n");
        klog_print_hex(soft_count);
        klog_print("\n");
        if (proc_count == 0){
            klog_print("SH: Spengo tutto amici\n");
            HALT(); // Spegne il computer
        }
        else if (soft_count > 0)
        {
            klog_print("SH soft_count > 0\n");
            // Imposto lo stato corrente per accettare interrupt
            // E disabilito il tutto il resto (quindi anche il PLT)
            unsigned int waitingStatus = getSTATUS() | IECON | IEPON | IMON;
            klog_print("SH - setState accetto interrupt\n");
            setTIMER(MAX_TIME);
            current_p = NULL;
            setSTATUS(waitingStatus);
            klog_print("SH - vado in Wait\n");
            WAIT(); // Aspettando un interrupt
            scheduler();
        }
        else
        {
            klog_print("SH - aaaaa panicdeadlock\n");
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
    klog_print("LOAD \n");
    LDST(&(current_p->p_s));
}
#include "SYSCALL_helpers.h"

// Funzioni Helper della SYSCALL Terminate_Process
void Exterminate(pcb_PTR process)
{
    // Controlla se il processo ha figli
    if (emptyChild(process))
    {
        /**
         * Nel caso il processo corrente sia figlio di qualche
         * altro processo, lo rimuove dalla lista dei figli del padre
         */
        outChild(process);
        // Termino il processo corrente
        freePcb(process);
        // Decremento il conto dei processi attivi
        proc_count--;
    }
    else
    {
        // Termina tutti i figli
        while (!emptyChild(process))
        {
            pcb_PTR child = removeChild(process);
            Terminate_Process(child->p_pid, 0, 0);
        }
        Terminate_Process(0, 0, 0); // Infine termino il processo originale
    }
}

pcb_PTR FindProcess(int pid)
{
    pcb_PTR iter = NULL;
    // Cerco nella lista dei processi ad alta prio
    list_for_each_entry(iter, high_ready_q, p_list)
    {
        if (iter->p_pid == pid)
            return iter;
    }
    // Cerco nella lista dei processi a bassa prio
    list_for_each_entry(iter, low_ready_q, p_list)
    {
        if (iter->p_pid == pid)
            return iter;
    }
    // Ricerca infruttuosa
    return NULL;
}
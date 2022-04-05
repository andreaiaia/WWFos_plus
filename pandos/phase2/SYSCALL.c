#include "SYSCALL.h";

extern int proc_count;

extern struct list_head *high_ready_q, *low_ready_q;

extern pcb_PTR current_p;

int Create_Process(state_t *statep, int prio, support_t *supportp)
{
    // Creo il processo figlio
    pcb_PTR child = allocPcb();
    // Se non è stato possibile allocare, ritorno errore
    if (child == NULL)
    {
        /**
         * Imposto il valore di fallimento nel registro v0
         * E avanto il PC di una WORDLEN per evitare il loop
         * di SYSCALL quando ripasso il controllo al chiamante
         */
        statep->reg_v0 = NOPROC;
        statep->pc_epc += WORDLEN;
        LDST(statep);
        return NOPROC;
    }
    else
    {
        // Imposto i campi secondo i parametri ricevuti
        child->p_s = *statep;
        child->p_prio = prio;
        child->p_supportStruct = supportp;
        /**
         * Devo dare un ID univoco al processo,
         * quindi gli do il suo indirizzo di memoria
         */
        child->p_pid = (memaddr)&child;

        // Associo il process appena creato al suo processo padre
        insertChild(current_p, child);

        // Inserisco il processo nella coda corretta)
        if (prio)
            insertProcQ(high_ready_q, child);
        else
            insertProcQ(low_ready_q, child);

        // Incremento il conto dei processi
        proc_count++;
    }

    // Vedi sopra
    statep->pc_epc += WORDLEN;
    statep->reg_v0 = OK;
    LDST(statep);

    return OK;
}

// Se il secondo parametro e’ 0 il bersaglio e’ il processo invocante
void Terminate_Process(int pid, 0, 0)
{
    if (pid == 0) // Termina il current_p
    {
        Exterminate(current_p);
    }
    else
    {
        Exterminate(FindProcess(pid));
    }

    // Rimando il controllo allo scheduler per attivare altri processi
    scheduler();
}

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
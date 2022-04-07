#include "SYSCALL.h";

/**
 * ! in tutte le funzioni seguenti aggiorno il PC del processo corrente
 * ! ma a pagina 33 del capitolo 3 parla di usare invece lo stato del processore
 * ! e performare una LDST modificando suddetto stato
 * ! Devo capire meglio questa parte
 */

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

        // Inserisco il processo nella coda corretta
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
void Terminate_Process(int pid)
{
    if (pid == 0)
    {
        exterminate(current_p); // Termina il current_p
        // Avanzo il PC
        current_p->p_s.pc_epc += WORDLEN;
    }
    else
    {
        pcb_PTR to_terminate = find_process(pid);
        exterminate(to_terminate); // Termina il proc con il corrispondente pid
        // Avanzo il PC
        to_terminate->p_s.pc_epc += WORDLEN;
    }

    // Rimando il controllo allo scheduler per attivare altri processi
    scheduler();
}

void Passeren(int *semaddr)
{
    if (*semaddr == NULL)
        return;
    if (*semaddr > 0)
        (*semaddr)--;
    else
    {
        // Blocco il processo e chiamo lo scheduler
        insertBlocked(semaddr, current_p);
        // Avanzo il PC
        current_p->p_s.pc_epc += WORDLEN;

        scheduler();
    }
}

void Verhogen(int *semaddr)
{
    pcb_PTR first = headBlocked(semaddr);

    if (first == NULL)
        (*semaddr)++;
    else
    {
        outBlocked(first);
        if (first->p_prio == 1)
            insertProcQ(high_ready_q, first);
        else
            insertProcQ(low_ready_q, first);

        // Avanzo il PC del processo che ha chiamato la SYSCALL
        current_p->p_s.pc_epc += WORDLEN;

        scheduler();
    }
}

// TODO Capire come trovare il processo chiamante delle SYSCALL
int Do_IO_Device(int *commandAddr, int commandValue)
{
    // Cerco il semaforo associato al processo corrente
    int *semaddr = NULL;
    int dev_position;
    /**
     * Parto da 32 poiché ci interessano solo i semafori dei terminal
     * devices, che sono gli ultimi 16 (non contando il 49esimo)
     */
    for (int i = 32; i < DEVSEM_NUM - 1; i++)
    {
        if (&device_sem[i] == current_p->p_semAdd)
        {
            semaddr = &device_sem[i];
            dev_position = i;
            break;
        }
    }
    /**
     * Una volta trovato il semaforo, blocco il processo che ha chiamato
     * la SYSCALL, aka il current_p
     */
    Passeren(semaddr);
    // Sapendo chi sia il semaforo che mi serve, possiamo trovare il terminale...
    devregarea_t *dev_regs = (devregarea_t *)RAMBASEADDR;
    // (Trovo il numero del terminale dividendo per 2 il dev_position salvato prima)
    int term_num = (dev_position - 32) / 2 + (dev_position - 32) % 2;
    termreg_t term = dev_regs->devreg[4][term_num].term; // perché 4?
    /**
     * ...E ricavare il campo status del terminale trovato
     * discriminando se era un semaforo di recv o uno di transm
     */
    int return_value;
    if (dev_position % 2 == 1)
        return_value = term.transm_status;
    else
        return_value = term.recv_status;

    *commandAddr = commandValue;

    // Restituisco lo status
    return return_value;
}

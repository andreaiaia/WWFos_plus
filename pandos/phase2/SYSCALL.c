#include "SYSCALL.h";

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
void Terminate_Process(int pid, 0, 0)
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

void Passeren(int *semaddr, 0, 0)
{
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

void Verhogen(int *semaddr, 0, 0)
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
int Do_IO_Device(int *commandAddr, int commandValue, 0)
{
    // 1. Cerco il range di indirizzi in cui si trova il commandAddr

    // 2. In base a ciò conosco l'interrupt line e il device Number da cui trovare il semaforo del device

    // 3. Sapendo il range di indirizzi del device register, possiamo ricavare il campo status

    // 4. Restituisco lo status

    // commandAddr = indirizzo dove mettere il commandValue alla fine

    // TODO: Avanza il PC
    scheduler();
}

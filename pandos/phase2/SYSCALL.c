#include "SYSCALL.h";

// TODO Rimuovere le chiamate allo scheduler e metterle nello switch case del SYSCALL Handler

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
        current_p->p_s.pc_epc += WORDLEN;
    }
}

void Passeren(int *semaddr)
{
    if (*semaddr == NULL)
        return;
    if (*semaddr > 0)
        (*semaddr)--;
    else
    {
        // Blocco il processo corrente e chiamo lo scheduler
        insertBlocked(semaddr, current_p);
        // Avanzo il PC
        current_p->p_s.pc_epc += WORDLEN;

        scheduler(); // ! qui credo che lo scheduler ci vada comunque
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

        // Avanzo il PC del processo corrente
        current_p->p_s.pc_epc += WORDLEN;
    }
}

void Do_IO_Device(int *commandAddr, int commandValue)
{
    // Cerco il semaforo associato al processo corrente
    int *semaddr = NULL;
    /**
     * Per trovare il dispositivo a cui è associato il
     * commandAddr ricevuto uso una macro che ho definito
     * in SYSCALL.h
     */
    int dev_position = DEV_POSITION(commandAddr);

    // Identifico line e dispositivo
    int line = 0;
    int dev = 0;
    if (dev_position > 31)
    {
        line = 4;
        // TODO come faccio a capire se transm o recv?
    }
    else
    {
        line = dev_position / 8;
        dev = dev_position - 8 * line;
    }

    // Faccio PASSEREN su dispositivo trovato
    Passeren(&(device_sem[dev_position]));

    // Scrivo nel commandAddr il valore ricevuto
    *commandAddr = commandValue;

    // Abilito gli Interrupt nel processo corrente
    current_p->p_s.status = (current_p->p_s.status) | IEPON | IMON;

    // Avanzo il PC per evitare il SYSCALL loop
    current_p->p_s.pc_epc += WORDLEN;
}

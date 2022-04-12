#include "SYSCALL.h"

// TODO Rimuovere le chiamate allo scheduler e metterle nello switch case del SYSCALL Handler
// TODO AVANZARE IL PROGRAM COUNTER NELL'EXCEPTION HANDLER
// il comando per farlo è
// current_p->p_s.pc_epc += WORDLEN;

void Create_Process(state_t *statep, int prio, support_t *supportp)
{
    // Creo il processo figlio
    pcb_PTR child = allocPcb();
    // Se non è stato possibile allocare, ritorno errore
    if (child == NULL)
    {
        /**
         * Imposto il valore di fallimento nel registro v0
         * E avanto il PC di una WORDLEN per evitare il loop
         * di SYSCALL, quindi ripasso il controllo al chiamante
         */
        statep->reg_v0 = NOPROC;
        // statep->pc_epc += WORDLEN;
        LDST(statep);
    }
    else
    {
        // Imposto i campi secondo i parametri ricevuti
        copy_state(statep, &(child->p_s));
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
    // statep->pc_epc += WORDLEN;
    statep->reg_v0 = OK;
    LDST(statep);
}

// Se il secondo parametro e’ 0 il bersaglio e’ il processo invocante
void Terminate_Process(int pid)
{
    if (pid == 0)
        Exterminate(current_p); // Termina il current_p
    else
    {
        pcb_PTR to_terminate = find_process(pid);
        Exterminate(to_terminate); // Termina il proc con il corrispondente pid
    }
}

void Passeren(int *semaddr)
{
    if (semaddr == NULL)
        return;
    else if (*semaddr == 0)
    {
        // Aggiungo il processo corrente alla coda del semd
        insertBlocked(semaddr, current_p);
        // Blocco il processo corrente
        current_p = NULL;
        // Chiamo lo scheduler
        scheduler();
    }
    else if (headBlocked(semaddr) != NULL)
    {
        pcb_PTR first = removeBlocked(semaddr);

        if (first->p_prio == 1)
            insertProcQ(high_ready_q, first);
        else
            insertProcQ(low_ready_q, first);

        // Blocco il processo corrente
        current_p = NULL;
        // Chiamo lo scheduler
        scheduler();
    }
    else
    {
        (*semaddr)--;
    }
}

void Verhogen(int *semaddr)
{
    if (*semaddr == 1)
    {
        // Blocco il processo corrente
        if (current_p->p_prio == 1)
            insertProcQ(high_ready_q, current_p);
        else
            insertProcQ(low_ready_q, current_p);
        current_p = NULL;
    }
    else if (headBlocked(semaddr) != NULL)
    {
        pcb_PTR first = removeBlocked(semaddr);
        if (first->p_prio == 1)
            insertProcQ(high_ready_q, first);
        else
            insertProcQ(low_ready_q, first);
    }
    else
        (*semaddr)++;
}

void Do_IO_Device(int *commandAddr, int commandValue)
{
    /**
     * Per trovare il dispositivo a cui è associato il
     * commandAddr ricevuto uso una macro che ho definito
     * in SYSCALL.h
     */
    int dev_position = DEV_POSITION(commandAddr);
    // Distinguo fra terminal dev e tutti gli altri dispositivi
    if (dev_position > 63)
        dev_position -= 32;
    else
        dev_position = dev_position / 2;

    // Faccio PASSEREN su dispositivo trovato
    Passeren(&(device_sem[dev_position]));

    // Scrivo nel commandAddr il valore ricevuto
    *commandAddr = commandValue;

    // Abilito gli Interrupt nel processo corrente
    current_p->p_s.status = (current_p->p_s.status) | IEPON | IMON;
}

void Get_CPU_Time()
{
    cpu_t elapsed = 0;
    // current_p->p_time + tempo trascorso dall'ultimo time slice
    STCK(elapsed);
    // Calcolo e ritorno il tempo trascorso trascorso da quando il processo è attivo
    current_p->p_s.reg_v0 = (unsigned int)(current_p->p_time + (elapsed - start));
}

void Wait_For_Clock()
{
    Passeren(&(device_sem[48]));
}

void Get_Support_Data()
{
    current_p->p_s.reg_v0 = (unsigned int)(current_p->p_supportStruct);
}

void Get_Process_Id(int parent)
{
    if (parent == 0)
        current_p->p_s.reg_v0 = (unsigned int)(current_p->p_pid);
    else
        current_p->p_s.reg_v0 = (unsigned int)((current_p->p_parent)->p_pid);
}

void Yield()
{
    if (current_p->p_prio == 1)
        insertProcQ(high_ready_q, current_p);
    else
        insertProcQ(low_ready_q, current_p);

    current_p = NULL;
}
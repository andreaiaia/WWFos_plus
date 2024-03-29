#include "SYSCALL.h"

void Create_Process(state_t *statep, int prio, support_t *supportp)
{
    // Creo il processo figlio
    pcb_PTR child = allocPcb();
    // Se non è stato possibile allocare, ritorno errore
    if (child == NULL)
        PROCESSOR_SAVED_STATE->reg_v0 = NOPROC; // Ritorno il valore di fallimento
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
        child->p_pid = (memaddr)child;
        // Associo il process appena creato al suo processo padre
        insertChild(current_p, child);
        // Inserisco il processo nella coda corretta
        if (prio)
            insertProcQ(&high_ready_q, child);
        else
            insertProcQ(&low_ready_q, child);

        // Incremento il conto dei processi
        proc_count++;
        for (int i = 0; i < MAXPROC; i++)
        {
            if (all_processes[i] == NULL)
            {
                all_processes[i] = child;
                break;
            }
        }
    }
    // Restituisco il valore di successo
    PROCESSOR_SAVED_STATE->reg_v0 = child->p_pid;
}

int Terminate_Process(int pid)
{
    if (pid == 0)
    {
        // Fermo il calcolo del tempo usato dal current_p
        STCK(finish);
        // Aggiungo il tempo trascorso al tempo impiegato dal processo
        current_p->p_time = (current_p->p_time) + (finish - start);
        // Termina il current_p
        Exterminate(current_p);
        current_p = NULL;
        return 0;
    }
    else
    {
        pcb_PTR to_terminate = find_process(pid);
        Exterminate(to_terminate); // Termina il proc con il corrispondente pid
        return 1;
    }
}

int Passeren(int *semaddr)
{
    if (semaddr == NULL)
        return 1;
    else if (*semaddr == 0)
    {
        // Fermo il calcolo del tempo usato dal current_p
        STCK(finish);
        // Aggiungo il tempo trascorso al tempo impiegato dal processo
        current_p->p_time = (current_p->p_time) + (finish - start);
        // Blocco il processo corrente
        insertBlocked(semaddr, current_p);
        return 0;
    }
    else if (headBlocked(semaddr) != NULL)
    {
        pcb_PTR first = removeBlocked(semaddr);
        first->p_semAdd = NULL;

        if (first->p_prio == 1)
            insertProcQ(&high_ready_q, first);
        else
            insertProcQ(&low_ready_q, first);

        return 1;
    }
    else
    {
        *semaddr = 0;
        return 1;
    }
}

pcb_PTR Verhogen(int *semaddr)
{
    pcb_PTR first = NULL;
    if (*semaddr == 1)
    {
        // Fermo il calcolo del tempo usato dal current_p
        STCK(finish);
        // Aggiungo il tempo trascorso al tempo impiegato dal processo
        current_p->p_time = (current_p->p_time) + (finish - start);
        //  Sospendo il processo corrente
        copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
        insertBlocked(semaddr, current_p);
        current_p = NULL;
    }
    else if (headBlocked(semaddr) != NULL)
    {
        first = removeBlocked(semaddr);

        if (first->p_prio == 1)
            insertProcQ(&high_ready_q, first);
        else
            insertProcQ(&low_ready_q, first);
    }
    else
        *semaddr = 1;

    return first;
}

int Do_IO_Device(int *commandAddr, int commandValue)
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
        dev_position /= 2;
    // Faccio PASSEREN su dispositivo trovato
    int retValue = Passeren(&(device_sem[dev_position]));
    //  Scrivo nel commandAddr il valore ricevuto
    *commandAddr = commandValue;
    // Restituisco il risultato della Passeren di poco sopra
    return retValue;
}

void Get_CPU_Time()
{
    cpu_t elapsed;
    STCK(elapsed);
    // Calcolo e ritorno il tempo trascorso trascorso da quando il processo è attivo
    PROCESSOR_SAVED_STATE->reg_v0 = (unsigned int)(current_p->p_time + (elapsed - start));
}

int Wait_For_Clock()
{
    return (Passeren(&(device_sem[48])));
}

void Get_Support_Data()
{
    PROCESSOR_SAVED_STATE->reg_v0 = (unsigned int)(current_p->p_supportStruct);
}

void Get_Process_Id(int parent)
{
    if (parent == 0)
        PROCESSOR_SAVED_STATE->reg_v0 = (int)(current_p->p_pid);
    else if (current_p->p_parent)
        PROCESSOR_SAVED_STATE->reg_v0 = (int)((current_p->p_parent)->p_pid);
    else
        PROCESSOR_SAVED_STATE->reg_v0 = 0;
}

void Yield()
{
    if (current_p->p_prio == 0)
        insertProcQ(&low_ready_q, current_p);
    else
    {
        if (emptyProcQ(&high_ready_q))
            yielded = current_p;
        else
            insertProcQ(&high_ready_q, current_p);
    }
}
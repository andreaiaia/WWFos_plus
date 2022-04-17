#include "SYSCALL.h"

void Create_Process(state_t *statep, int prio, support_t *supportp)
{
    // Creo il processo figlio
    pcb_PTR child = allocPcb();
    // Se non è stato possibile allocare, ritorno errore
    if (child == NULL)
        current_p->p_s.reg_v0 = NOPROC; // Ritorno il valore di fallimento
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
            insertProcQ(&high_ready_q, child);
        else
            insertProcQ(&low_ready_q, child);

        // Incremento il conto dei processi
        proc_count++;
        for (int i = 0; i < MAXPROC; i++)
        {
            if (all_processes[i] == NULL)
                all_processes[i] = child;
        }
    }
    // Ritorno successo
    current_p->p_s.reg_v0 = OK;
}

// Se il secondo parametro è 0 allora il bersaglio è il processo invocante
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
    klog_print("PASS\n");
    if (semaddr == NULL)
    {
        klog_print("PASS1\n");
        return;
    }
    else if (*semaddr == 0)
    {
        klog_print("PASS2\n");
        //* Blocco il processo corrente
        // Aggiungo il processo corrente alla coda del semd
        insertBlocked(semaddr, current_p);
        soft_count++;
    }
    else if (headBlocked(semaddr) != NULL)
    {
        klog_print("PASS3\n");
        pcb_PTR first = removeBlocked(semaddr);
        soft_count--;

        if (first->p_prio == 1)
            insertProcQ(&high_ready_q, first);
        else
            insertProcQ(&low_ready_q, first);
    }
    else
    {
        klog_print("PASS4\n");
        (*semaddr)--;
    }
}

pcb_PTR Verhogen(int *semaddr)
{
    klog_print("VER\n");
    pcb_PTR first = NULL;

    if (*semaddr == 1)
    {
        klog_print("VER1\n");
        // Sospendo il processo corrente
        if (current_p->p_prio == 1)
            insertProcQ(&high_ready_q, current_p);
        else
            insertProcQ(&low_ready_q, current_p);
    }
    else if (headBlocked(semaddr) != NULL)
    {
        klog_print("VER2\n");
        first = removeBlocked(semaddr);
        soft_count--;
        if (first->p_prio == 1)
            insertProcQ(&high_ready_q, first);
        else
            insertProcQ(&low_ready_q, first);
    }
    else
    {
        klog_print("VER3\n");
        (*semaddr)++;
    }
    klog_print("VER4\n");
    return first;
}

void Do_IO_Device(int *commandAddr, int commandValue)
{
    klog_print("DOIO\n");
    /**
     * Per trovare il dispositivo a cui è associato il
     * commandAddr ricevuto uso una macro che ho definito
     * in SYSCALL.h
     */
    int dev_position = find_dev(commandAddr);

    // Distinguo fra terminal dev e tutti gli altri dispositivi
    // if (dev_position > 32)
    //     dev_position = FIND_TERM(commandAddr);

    // Faccio PASSEREN su dispositivo trovato
    klog_print_hex(dev_position);
    klog_print("\n");
    Passeren(&(device_sem[dev_position]));
    klog_print("DOIO1\n");
    // Scrivo nel commandAddr il valore ricevuto
    *commandAddr = commandValue;

    // Abilito gli Interrupt nei device
    // current_p->p_s.status = (current_p->p_s.status) | IEPON | IMON;
}

void Get_CPU_Time()
{
    cpu_t elapsed;
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
    if (current_p->p_prio == 0)
    {
        insertProcQ(&low_ready_q, current_p);
        current_p = NULL;
    }
    else
    {
        if (emptyProcQ(&high_ready_q))
        {
            yielded = current_p;
            current_p = NULL;
        }
        else
        {
            insertProcQ(&high_ready_q, current_p);
            current_p = NULL;
        }
    }
}
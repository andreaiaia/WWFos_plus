#include "helpers.h"

/* Helpers Generici */

void copy_state(state_t *original, state_t *dest)
{
    dest->entry_hi = original->entry_hi;
    dest->cause = original->cause;
    dest->status = original->status;
    dest->pc_epc = original->pc_epc;
    dest->hi = original->hi;
    dest->lo = original->lo;
    for (int i = 0; i < STATE_GPR_LEN; i++)
    {
        dest->gpr[i] = original->gpr[i];
    }
}

/* Helpers per le SYSCALL */

//* "Do you know how you make someone into a Dalek? Subtract Love, add Anger." ~ Steven Moffat
void Exterminate(pcb_PTR process)
{
    // Controlla se il processo non ha figli
    if (!emptyChild(process))
    {
        // Termina tutti i figli
        while (!emptyChild(process))
        {
            pcb_PTR child = removeChild(process);
            Exterminate(child);
        }
    }
    /**
     * Nel caso il processo corrente sia figlio di qualche
     * altro processo, lo rimuove dalla lista dei figli del padre
     */
    outChild(process);
    // Riduciamo il soft_count solo se il proc è bloccato da un dev sem
    for (int i = 0; i < DEVSEM_NUM; i++)
    {
        if (process->p_semAdd == &device_sem[i])
            soft_count--;
    }
    // Rimuoviamo il proc da qualunque coda possa trovarsi
    if (process->p_prio)
        outProcQ(&high_ready_q, process);
    else
        outProcQ(&low_ready_q, process);

    // Togliamo il processo dalla coda dell'eventuale semaforo
    outBlocked(process);
    // Decremento il conto dei processi attivi
    proc_count--;
    // Rimuovo il processo dall'array di tutti i processi
    for (int i = 0; i < MAXPROC; i++)
    {
        if (all_processes[i] == process)
            all_processes[i] = NULL;
    }
    // Termino il processo corrente
    freePcb(process);
}

pcb_PTR find_process(int pid)
{
    if (current_p->p_pid == pid)
        return current_p;
    else
    {
        for (int i = 0; i < MAXPROC; i++)
        {
            if ((all_processes[i]) && (all_processes[i]->p_pid == pid))
                return all_processes[i];
        }
    }
    /**
     * Qui non arriva mai, ma lo lasciamo per evitare
     * warning dal compilatore
     */
    return NULL;
}

void syscallExceptionHandler(unsigned int syscallCode)
{
    /**
     * Verifico che il processo chiamante della Syscall
     * sia in KernelMode e che abbia chiamato una Syscall (rega0 < 0)
     */
    if (((PROCESSOR_SAVED_STATE->status & STATUS_KUp) != STATUS_KUp) && ((int)syscallCode < 0))
    {
        /**
         * Syscall lecita, ovvero processo in modalità Kernel e parametro a0 negativo.
         * Procedo a smistare alla syscall corretta basandomi sul syscallCode
         */
        switch (syscallCode)
        {
        case CREATEPROCESS:
            INCREMENTO_PC;
            Create_Process((state_t *)(REG_A1_SS), (int)(REG_A2_SS), (support_t *)(REG_A3_SS));
            load_state();
            break;

        case TERMPROCESS:
            INCREMENTO_PC;

            if (Terminate_Process((int)(REG_A1_SS)))
                load_state();

            break;

        case PASSEREN:
            INCREMENTO_PC;

            if (Passeren((int *)(REG_A1_SS)))
                load_state();
            else
                copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));

            break;

        case VERHOGEN:
            INCREMENTO_PC;

            Verhogen((int *)(REG_A1_SS));
            if (current_p != NULL)
                load_state();

            break;

        case DOIO:
            INCREMENTO_PC;
            Do_IO_Device((int *)(REG_A1_SS), (int)REG_A2_SS);
            // Dopo la DOIO si va sempre in WAIT
            copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
            soft_count++;
            break;

        case GETTIME:
            INCREMENTO_PC;
            Get_CPU_Time();
            load_state();
            break;

        case CLOCKWAIT:
            INCREMENTO_PC;
            Wait_For_Clock();
            // Dopo la CLOCKWAIT si va sempre nello scheduler
            copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
            soft_count++;
            break;

        case GETSUPPORTPTR:
            INCREMENTO_PC;
            Get_Support_Data();
            load_state();
            break;

        case GETPROCESSID:
            INCREMENTO_PC;
            Get_Process_Id((int)(REG_A1_SS));
            load_state();
            break;

        case YIELD:
            INCREMENTO_PC;
            Yield();
            copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
            current_p = NULL;
            break;
        }

        scheduler();
    }
    // Caso in cui la syscall non è lecita
    else if ((int)syscallCode > 0)
        PassUpOrDie(GENERALEXCEPT);
    else
    {
        // Imposto il bit RI
        PROCESSOR_SAVED_STATE->cause = (PROCESSOR_SAVED_STATE->cause & ~CAUSE_EXCCODE_MASK) | (EXC_RI << CAUSE_EXCCODE_BIT);
        // Simulo una TRAP
        PassUpOrDie(GENERALEXCEPT);
    }
}

void PassUpOrDie(int excCode)
{
    klog_print("sono in una passup");
    /**
     * Se la supportStruct è nulla si entra nella "Die"
     * e si termina il processo corrente e tutta la sua progenie.
     * Altrimenti si esegue la "Pass Up" e si inoltra la richiesta
     * al livello di supporto (prossima fase del progetto).
     */
    if (current_p->p_supportStruct == NULL)
    {
        Terminate_Process(0);
        scheduler();
    }
    else
    {
        //  Copio l'exception state
        copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_supportStruct->sup_exceptState[excCode]));
        //  Copio stack pointer, status e program counter
        unsigned int stack_ptr = current_p->p_supportStruct->sup_exceptContext[excCode].stackPtr;
        unsigned int status = current_p->p_supportStruct->sup_exceptContext[excCode].status;
        unsigned int pc = current_p->p_supportStruct->sup_exceptContext[excCode].pc;
        // Carico il nuovo contesto nel processo attivo
        LDCXT(stack_ptr, status, pc);
    }
}

/**
 * Dopo ogni Syscall in genere bisogna o ritornare al processo
 * che era in esecuzione al momento della chiamata della Syscall
 * oppure (se il processo è stato bloccato o terminato) chiamare
 * lo scheduler per lanciare un altro processo.
 */
void load_state()
{
    if (current_p != NULL)
    {
        // Fermo il calcolo del tempo usato dal current_p
        STCK(finish);
        // Aggiungo il tempo trascorso al tempo impiegato dal processo
        current_p->p_time = (current_p->p_time) + (finish - start);
    }
    STCK(start);
    LDST(PROCESSOR_SAVED_STATE);
}
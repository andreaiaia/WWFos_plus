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

/**
 * La funzione ritorna il processo sbloccato o
 * NULL altrimenti
 */
pcb_PTR P(int *semaddr)
{
    pcb_PTR first = NULL;
    klog_print("P\n");
    if (*semaddr == 0)
    {
        klog_print("P1\n");
        //* Blocco il processo corrente
        // Aggiungo il processo corrente alla coda del semd
        insertBlocked(semaddr, current_p);
        soft_count++;
    }
    else if (headBlocked(semaddr) != NULL)
    {
        klog_print("P2\n");
        first = removeBlocked(semaddr);
        soft_count--;

        if (first->p_prio == 1)
            insertProcQ(&high_ready_q, first);
        else
            insertProcQ(&low_ready_q, first);
    }
    else
    {
        klog_print("P3\n");
        (*semaddr)--;
    }

    return first;
}

/* Helpers per le SYSCALL */

//* "Do you know how you make someone into a Dalek? Subtract Love, add Anger." ~ Steven Moffat
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
        for (int i = 0; i < MAXPROC; i++)
        {
            if (all_processes[i] == process)
                all_processes[i] = NULL;
        }
    }
    else
    {
        // Termina tutti i figli
        while (!emptyChild(process))
        {
            pcb_PTR child = removeChild(process);
            Terminate_Process(child->p_pid);
        }
        Terminate_Process(0); // Infine termino il processo originale
    }
}

pcb_PTR find_process(int pid)
{
    if (current_p->p_pid == pid)
        return current_p;
    else
    {
        for (int i = 0; i < MAXPROC; i++)
        {
            if (all_processes[i]->p_pid == pid)
            {
                return all_processes[i];
            }
        }
    }

    return NULL;
}

void syscallExceptionHandler(unsigned int syscallCode)
{
    klog_print("HELP1\n");
    // * Verifico che il processo chiamante della Syscall sia in KernelMode e che abbia chiamato una Syscall (rega0 < 0)
    if (((PROCESSOR_SAVED_STATE->status & STATUS_KUp) != STATUS_KUp))
    {
        klog_print("HELP2\n");
        INCREMENTO_PC;
        // * Syscall lecita, ovvero processo in modalità Kernel e parametro a0 negativo.
        // * Procedo a smistare alla syscall corretta basandomi sul syscallCode
        klog_print("HELP2.1\n");
        switch (syscallCode)
        {
        case CREATEPROCESS:
            klog_print("HELP2.2\n");
            Create_Process((state_t *)(REG_A1_SS), (int)(REG_A2_SS), (support_t *)(REG_A3_SS));
            break;

        case TERMPROCESS:
            klog_print("HELP2.3\n");
            Terminate_Process((int)(REG_A1_SS));
            break;

        case PASSEREN:
            klog_print("HELP2.4\n");
            Passeren((int *)(REG_A1_SS));
            break;

        case VERHOGEN:
            klog_print("HELP2.5\n");
            Verhogen((int *)(REG_A1_SS));
            break;

        case DOIO:
            klog_print("HELP2.6\n");
            Do_IO_Device((int *)(REG_A1_SS), (int)REG_A2_SS);
            break;

        case GETTIME:
            klog_print("HELP2.7\n");
            Get_CPU_Time();
            break;

        case CLOCKWAIT:
            klog_print("HELP2.8\n");
            Wait_For_Clock();
            break;

        case GETSUPPORTPTR:
            klog_print("HELP2.9\n");
            Get_Support_Data();
            break;

        case GETPROCESSID:
            klog_print("HELP2.10\n");
            Get_Process_Id((int)(REG_A1_SS));
            break;

        case YIELD:
            klog_print("HELP2.11\n");
            Yield();
            break;

        default:
            klog_print("HELP2.12\n");
            // * Imposto il bit RI
            PROCESSOR_SAVED_STATE->cause = (PROCESSOR_SAVED_STATE->cause & ~CAUSE_EXCCODE_MASK) | (EXC_RI << CAUSE_EXCCODE_BIT);
            // * Simulo una TRAP
            PassUpOrDie(GENERALEXCEPT);
            break;
        }
        scheduler();
    }
    // * Caso in cui la syscall non è lecita
    else
    {
        klog_print("HELP3\n");
        // * Imposto il bit RI
        PROCESSOR_SAVED_STATE->cause = (PROCESSOR_SAVED_STATE->cause & ~CAUSE_EXCCODE_MASK) | (EXC_RI << CAUSE_EXCCODE_BIT);
        // * Simulo una TRAP
        klog_print("HELP4\n");
        PassUpOrDie(GENERALEXCEPT);
        klog_print("HELP5\n");
    }
}

/* PassUpOrDie */

void PassUpOrDie(int excCode)
{
    /**
     * Se la supportStruct è nulla si entra nella "Die"
     * e si termina il processo corrente e tutta la sua progenie.
     * Altrimenti si esegue la "Pass Up" e si inoltra la richiesta
     * al livello di supporto (prossima fase del progetto).
     */
    klog_print("HELP6\n");
    klog_print("HELP6BIS\n");
    klog_print("HELP6TRIS\n");
    if (current_p->p_supportStruct == NULL)
    {
        klog_print("HELP7\n");
        Terminate_Process(0);
    }
    else
    {
        klog_print("HELP8\n");
        // Copio l'exception state
        copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_supportStruct->sup_exceptState[excCode]));
        klog_print("HELP9\n");
        // Copio stack pointer, status e program counter
        unsigned int stack_ptr = current_p->p_supportStruct->sup_exceptContext[excCode].stackPtr;
        unsigned int status = current_p->p_supportStruct->sup_exceptContext[excCode].status;
        unsigned int pc = current_p->p_supportStruct->sup_exceptContext[excCode].pc;
        // Carico il nuovo contesto nel processo attivo
        LDCXT(stack_ptr, status, pc);
        klog_print("HELP10\n");
    }
    klog_print("HELPEND\n");
}
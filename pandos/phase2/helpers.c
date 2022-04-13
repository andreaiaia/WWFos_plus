#include "helpers.h"

// Funzioni Helper della SYSCALL Terminate_Process
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

void post_syscall() {
    LDST((PROCESSOR_SAVED_STATE));
    scheduler();
}

void syscallExceptionHandler(unsigned int syscallCode) {
    // * Verifico che il processo chiamante della Syscall sia in KernelMode e che abbia chiamato una Syscall (rega0 < 0)
    if (((PROCESSOR_SAVED_STATE->status & STATUS_KUp) == STATUS_KUp) && syscallCode < 0) {
        INCREMENTO_PC;    
        // * Syscall lecita, ovvero processo in modalità Kernel e parametro a0 negativo.
        // * Procedo a smistare alla syscall corretta basandomi sul syscallCode
        switch (syscallCode) {
            case -1:
                Create_Process((state_t *)(REG_A1_SS), (int)(REG_A2_SS), (support_t *)(REG_A3_SS));
                post_syscall();
                break;

            case -2:
                Terminate_Process((int)(REG_A1_SS));                
                post_syscall();
                break;

            case -3:
                Passeren((int *)(REG_A1_SS));                
                post_syscall();
                break;

            case -4:
                Verhogen((int *)(REG_A1_SS));                
                post_syscall();
                break;

            case -5:
                Do_IO_Device((int *)(REG_A1_SS), (int)REG_A2_SS);               
                post_syscall();
                break;

            case -6:
                Get_CPU_Time();                
                post_syscall();
                break;

            case -7:
                Wait_For_Clock();                
                post_syscall();
                break;

            case -8:
                Get_Support_Data();                
                post_syscall();
                break;

            case -9:
                Get_Process_Id((int)(REG_A1_SS));                
                post_syscall();
                break;

            case -10:
                Yield();             
                post_syscall();
                break;
        }
    }
        // * Caso in cui la syscall non è lecita 
        else
        // * Imposto il bit RI
        PROCESSOR_SAVED_STATE->cause = (PROCESSOR_SAVED_STATE->cause & ~CAUSE_EXCCODE_MASK) | (EXC_RI << CAUSE_EXCCODE_BIT);
        // * Simulo una TRAP
        PassUpOrDie(GENERALEXCEPT);

}

void PassUpOrDie(int excCode) {
    /**
     * Se la supportStruct è nulla si entra nella "Die"
     * e si termina il processo corrente e tutta la sua progenie.
     * Altrimenti si esegue la "Pass Up" e si inoltra la richiesta
     * al livello di supporto (prossima fase del progetto).
     */
    if (current_p->p_supportStruct == NULL)
        Terminate_Process(0);
    else
    {
        // Copio l'exception state
        copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_supportStruct->sup_exceptState[excCode]));
        // Copio stack pointer, status e program counter
        unsigned int stack_ptr = current_p->p_supportStruct->sup_exceptContext[excCode].stackPtr;
        unsigned int status = current_p->p_supportStruct->sup_exceptContext[excCode].status;
        unsigned int pc = current_p->p_supportStruct->sup_exceptContext[excCode].pc;
        // Carico il nuovo contesto nel processo attivo
        LDCXT(stack_ptr, status, pc);
    }
}
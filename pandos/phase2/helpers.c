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
        //Flag usato come guardia, nel caso Flag venga azzerato, il semaforo era di tipo DEVICE.
        int flag = 1;
        for (int i=0; i < DEVSEM_NUM; i++) {
            if (process->p_semAdd == &device_sem[i]) {
                soft_count--;
                flag=0;
            } 
        }
        if (process->p_prio) {
            outProcQ(&high_ready_q, process);
        }
        else {
            outProcQ(&low_ready_q, process);
        } // ! Possibile ottimizzazione col flag in base a risultato outprocq

        // Togliamo il processo dalla coda del semaforo
        outBlocked(process);
        // Decremento il conto dei processi attivi
        proc_count--;
        for (int i = 0; i < MAXPROC; i++)
        {
            if (all_processes[i] == process)
                all_processes[i] = NULL;
        }
        // Se flag è 1, l'if a riga 42 non ha trovato corrispondenza con un semaforo di tipo device
        if (flag) {
            // Se il semaforo non sta bloccando altri processi, incremento il suo valore.
                if (!headBlocked(process->p_semAdd)) {
                *(process->p_semAdd) = 1;
                }
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
            {
                return all_processes[i];
            }
        }
    }
    return NULL;
}

void syscallExceptionHandler(unsigned int syscallCode)
{
    klog_print("HELP1 - entro syscallExcHandler\n");
    // * Verifico che il processo chiamante della Syscall sia in KernelMode e che abbia chiamato una Syscall (rega0 < 0)
    if (((PROCESSOR_SAVED_STATE->status & STATUS_KUp) != STATUS_KUp))
    {
        klog_print("HELP2 - check ker. mode: ON\n");
        // * Syscall lecita, ovvero processo in modalità Kernel e parametro a0 negativo.
        // * Procedo a smistare alla syscall corretta basandomi sul syscallCode
        // ! klog_print("HELP2.1 - \n");   ho rimosso questo perché ridondante
        switch (syscallCode)
        {
        case CREATEPROCESS:
            INCREMENTO_PC;
            klog_print("HELP2.2 - create process\n");
            Create_Process((state_t *)(REG_A1_SS), (int)(REG_A2_SS), (support_t *)(REG_A3_SS));
            //LDST(PROCESSOR_SAVED_STATE);
            postSyscall();
            break;

        case TERMPROCESS:
            INCREMENTO_PC;
            klog_print("HELP2.3 - terminate process\n");
            if (Terminate_Process((int)(REG_A1_SS))){
                //LDST(PROCESSOR_SAVED_STATE);   
                postSyscall();
            } else {
                scheduler();
            }
            break;

        case PASSEREN:
            INCREMENTO_PC;
            klog_print("HELP2.4 - passeren\n");
            if(Passeren((int *)(REG_A1_SS))){
                //LDST(PROCESSOR_SAVED_STATE);
                postSyscall();
            } else {
                copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
            }
            klog_print("soft_count: ");
            klog_print_hex(soft_count);
            klog_print("\n");
            break;

        case VERHOGEN:
            INCREMENTO_PC;
            klog_print("HELP2.5 - verhogen\n");
            Verhogen((int *)(REG_A1_SS));
            if (current_p != NULL) {
                //LDST(PROCESSOR_SAVED_STATE);
                postSyscall();
            }
            break;

        case DOIO:
            INCREMENTO_PC;
            klog_print("HELP2.6 - DoIo cane\n");
            if(Do_IO_Device((int *)(REG_A1_SS), (int)REG_A2_SS)){
              soft_count--;
              //LDST(PROCESSOR_SAVED_STATE);
              postSyscall();  
            } else {
                copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
                soft_count++;
            }
            break;

        case GETTIME:
            INCREMENTO_PC;
            klog_print("HELP2.7 - gettime\n");
            Get_CPU_Time();
            //LDST(PROCESSOR_SAVED_STATE);
            postSyscall();
            break;

        case CLOCKWAIT:
            INCREMENTO_PC;
            klog_print("HELP2.8 - clock wait\n");
            if(Wait_For_Clock()) {
                soft_count--;
                //LDST(PROCESSOR_SAVED_STATE);
                postSyscall();
            } else {
                copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
                soft_count++;
            }
            break;

        case GETSUPPORTPTR:
            INCREMENTO_PC;
            klog_print("HELP2.9 - getsupportptr\n");
            Get_Support_Data();
            //LDST(PROCESSOR_SAVED_STATE);
            postSyscall();
            break;

        case GETPROCESSID:
            INCREMENTO_PC;
            klog_print("HELP2.10 - getprocID\n");
            Get_Process_Id((int)(REG_A1_SS));
            //LDST(PROCESSOR_SAVED_STATE);
            postSyscall();
            break;

        case YIELD:
            INCREMENTO_PC;
            klog_print("HELP2.11 - yield\n");
            Yield();
            copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
            current_p = NULL;
            break;

        default:
            klog_print("HELP2.12 - default case\n");
            // * Imposto il bit RI
            PROCESSOR_SAVED_STATE->cause = (PROCESSOR_SAVED_STATE->cause & ~CAUSE_EXCCODE_MASK) | (EXC_RI << CAUSE_EXCCODE_BIT);
            // * Simulo una TRAP
            PassUpOrDie(GENERALEXCEPT);
            break;
        }
        klog_print("HELP_ CHIAMATA SCHEDULERO\n");
        scheduler();
        klog_print("HELP_SE COMPAIO SO CAZZI AMARI\n");
    }
    // * Caso in cui la syscall non è lecita
    else
    {
        klog_print("HELP3 - syscall illecita\n");
        // * Imposto il bit RI
        PROCESSOR_SAVED_STATE->cause = (PROCESSOR_SAVED_STATE->cause & ~CAUSE_EXCCODE_MASK) | (EXC_RI << CAUSE_EXCCODE_BIT);
        // * Simulo una TRAP
        klog_print("HELP3.1 - chiamo passupordie\n");
        PassUpOrDie(GENERALEXCEPT);
        klog_print("HELP3.2 - passupordie fatta\n");
    }
}
void placeholder() {
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
    klog_print("HELP6 - Entro PassUpOrDie\n");
    if (current_p->p_supportStruct == NULL)
    {
        klog_print("HELP7 - supportStruct == NULL\n");
        Terminate_Process(0);
        scheduler();
    }
    else
    {
        klog_print("HELP8 - supportStruct != NULL\n");
        // Copio l'exception state
        copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_supportStruct->sup_exceptState[excCode]));
        klog_print("HELP9 - copiato excState\n");
        // Copio stack pointer, status e program counter
        unsigned int stack_ptr = current_p->p_supportStruct->sup_exceptContext[excCode].stackPtr;
        unsigned int status = current_p->p_supportStruct->sup_exceptContext[excCode].status;
        unsigned int pc = current_p->p_supportStruct->sup_exceptContext[excCode].pc;
        // Carico il nuovo contesto nel processo attivo
        LDCXT(stack_ptr, status, pc);
        klog_print("HELP10 - caricato contesto in proc attivo\n");
    }
    klog_print("HELPEND - fine passupordie\n");
}

void postSyscall() {
    STCK(finish);
    // Aggiungo il tempo trascorso al tempo impiegato dal processo
    current_p->p_time = (current_p->p_time) + (finish - start);
    STCK(start);
    LDST(PROCESSOR_SAVED_STATE);
}
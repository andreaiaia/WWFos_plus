#include "exception.h"

// extern pcb_PTR current_p;
//#define PROCESSO_CORRENTE current_p
//  CP0 Cause fields
//  #define CAUSE_EXCCODE_MASK     0x0000007c
//  #define CAUSE_EXCCODE_BIT      2
//  #define CAUSE_GET_EXCCODE(x)   (((x) & CAUSE_EXCCODE_MASK) >> CAUSE_EXCCODE_BIT)

// ? chi chiama exceptionHandler() ? Dal manuale:
// ? Therefore, if the Pass Up Vector was correctly initialized, fooBar will be
// ? called (with a fresh stack) after each and every exception, exclusive of TLBRefill
// ? events.
// ? probabilmente lo fa grazie alla riga nel main quando diamo al passupvector l'indirizzo dell'excp handler
void exceptionHandler()
{
    // * Exception code 0 -> Si passa il controllo al device interrupt handler
    // * Exception code 1-3 -> Si passa il controllo al TLB exception handler
    // * Exception code 4-7 9-12 -> Si passa il controllo al Program Trap exception handler
    // * Exception code 8 -> Si passa il controllo al SYSCALL exception handler
    if (CAUSE_GET_EXCCODE(stato_processo->cause) == 0)
    {
        interruptHandler();
    }
    else if ((CAUSE_GET_EXCCODE(stato_processo->cause) >= 1) && (CAUSE_GET_EXCCODE(stato_processo->cause) <= 3))
    {
        uTLB_RefillHandler();
    }
    else if (((CAUSE_GET_EXCCODE(stato_processo->cause) >= 4) && (CAUSE_GET_EXCCODE(stato_processo->cause) <= 7)) || ((CAUSE_GET_EXCCODE(stato_processo->cause) >= 9) && (CAUSE_GET_EXCCODE(stato_processo->cause) <= 12)))
    {
        // TODO NICK LO FA
        // ! Program trap exception handler
    }
    else if (CAUSE_GET_EXCCODE(stato_processo->cause) == 8)
    {
        // !SYSCALL EXCEPTION HANDLER
        // !a pagina 26 (9 del pdf)
        // ? Dal manuale abbiamo:
        // ? Furthermore, the processor state at the time of the exception
        // ? (the saved exception state) will have been stored (for Processor 0) at the start
        // ? of the BIOS Data Page (0x0FFF.F000).
        // TODO se il processo che fa una syscall è in kernel mode E
        // TODO a0 contiene un numero negativo, syscall, altrimenti i guess termina
        if (stato_processo->status == STATUS_KUp && stato_processo->reg_a0 < 0)
        {
            // Caso in cui la syscall è lecita, ovvero processo in modalità Kernel e parametro a0 negativo.
            switch (stato_processo->reg_a0)
            {

            case -1:
                Create_Process((state_t *)(stato_processo->reg_a1), (int)(stato_processo->reg_a2), (support_t *)(stato_processo->reg_a3));
                incremento_pc;
                scheduler();
                break;

            case -2:
                Terminate_Process((int)(stato_processo->reg_a1));
                incremento_pc;
                scheduler();
                break;

            case -3:
                Passeren((int *)(stato_processo->reg_a1));
                incremento_pc;
                scheduler();
                break;

            case -4:
                Verhogen((int *)(stato_processo->reg_a1));
                incremento_pc;
                scheduler();
                break;

            case -5:
                Do_IO_Device((int *)(stato_processo->reg_a1), (int)stato_processo->reg_a2);
                incremento_pc;
                scheduler();
                break;

            case -6:
                Get_CPU_Time();
                incremento_pc;
                scheduler();
                break;

            case -7:
                Wait_For_Clock();
                incremento_pc;
                scheduler();
                break;

            case -8:
                Get_Support_Data();
                incremento_pc;
                scheduler();
                break;

            case -9:
                Get_Process_Id((int *)(stato_processo->reg_a1));
                incremento_pc;
                scheduler();
                break;

            case -10:
                Yield();
                incremento_pc;
                scheduler();
                break;
            }
        }
        // Caso in cui la syscall non è lecita
        else
            PassUpOrDie(NULL);
    }
}

void PassUpOrDie(int excCode)
{
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
        copy_state(current_p->p_supportStruct->sup_exceptState[excCode], ((state_t *)BIOSDATAPAGE));
        // Copio stack pointer, status e program counter
        int stack_ptr = current_p->p_supportStruct->sup_exceptContext[excCode].stackPtr;
        int status = current_p->p_supportStruct->sup_exceptContext[excCode].status;
        int pc = current_p->p_supportStruct->sup_exceptContext[excCode].pc;
        // Carico il nuovo contesto nel processo attivo
        LDCXT(stack_ptr, status, pc);
    }
}
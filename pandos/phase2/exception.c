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
    if (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) == 0)
    {
        interruptHandler();
    }
    else if ((CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) >= 1) && (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) <= 3))
    {
        PassUpOrDie(PGFAULTEXCEPT);
    }
    else if (((CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) >= 4) && (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) <= 7)) || ((CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) >= 9) && (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) <= 12)))
    {

        PassUpOrDie(GENERALEXCEPT);
    }
    else if (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) == 8)
    {
        // !SYSCALL EXCEPTION HANDLER
        // !a pagina 26 (9 del pdf)
        // ? Dal manuale abbiamo:
        // ? Furthermore, the processor state at the time of the exception
        // ? (the saved exception state) will have been stored (for Processor 0) at the start
        // ? of the BIOS Data Page (0x0FFF.F000).
        // TODO se il processo che fa una syscall è in kernel mode E
        // TODO a0 contiene un numero negativo, syscall, altrimenti i guess termina
        if (STATO_PROCESSO->status == STATUS_KUp && STATO_PROCESSO->reg_a0 < 0)
        {
            // Caso in cui la syscall è lecita, ovvero processo in modalità Kernel e parametro a0 negativo.
            switch (STATO_PROCESSO->reg_a0)
            {

            case -1:
                Create_Process((state_t *)(STATO_PROCESSO->reg_a1), (int)(STATO_PROCESSO->reg_a2), (support_t *)(STATO_PROCESSO->reg_a3));
                INCREMENTO_PC;
                scheduler();
                break;

            case -2:
                Terminate_Process((int)(STATO_PROCESSO->reg_a1));
                INCREMENTO_PC;
                scheduler();
                break;

            case -3:
                Passeren((int *)(STATO_PROCESSO->reg_a1));
                INCREMENTO_PC;
                scheduler();
                break;

            case -4:
                Verhogen((int *)(STATO_PROCESSO->reg_a1));
                INCREMENTO_PC;
                scheduler();
                break;

            case -5:
                Do_IO_Device((int *)(STATO_PROCESSO->reg_a1), (int)STATO_PROCESSO->reg_a2);
                INCREMENTO_PC;
                scheduler();
                break;

            case -6:
                Get_CPU_Time();
                INCREMENTO_PC;
                scheduler();
                break;

            case -7:
                Wait_For_Clock();
                INCREMENTO_PC;
                scheduler();
                break;

            case -8:
                Get_Support_Data();
                INCREMENTO_PC;
                scheduler();
                break;

            case -9:
                Get_Process_Id((int)(STATO_PROCESSO->reg_a1));
                INCREMENTO_PC;
                scheduler();
                break;

            case -10:
                Yield();
                INCREMENTO_PC;
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
        copy_state(&(current_p->p_supportStruct->sup_exceptState[excCode]), ((state_t *)BIOSDATAPAGE));
        // Copio stack pointer, status e program counter
        int stack_ptr = current_p->p_supportStruct->sup_exceptContext[excCode].stackPtr;
        int status = current_p->p_supportStruct->sup_exceptContext[excCode].status;
        int pc = current_p->p_supportStruct->sup_exceptContext[excCode].pc;
        // Carico il nuovo contesto nel processo attivo
        LDCXT(stack_ptr, status, pc);
    }
}
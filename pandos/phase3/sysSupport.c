#include "sysSupport.h"
// General exception handler - Sezione 4.6
void generalExcHandler()
{
    // Prende il current process supp struct
    support_t *currSupStructPTR = (support_t *)SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    /*determina la causa*/
    int cause = CAUSE_GET_EXCCODE(currSupStructPTR->sup_exceptState[GENERALEXCEPT].cause);

    // Se e' una syscall chiama il syscall exception handler
    if (cause == SYSEXCEPTION)
        syscallExcHandler(currSupStructPTR);

    // Se non è una syscall, passo la gestione al trap exception handler
    trapExcHandler(currSupStructPTR);
}
/* the Support Level’s SYSCALL exception handler must also increment
the PC by 4 in order to return control to the instruction after the
SYSCALL instruction. */
// !Chiedere ad andrea

// SYSCALL exception handler - Sezione 4.7
void syscallExcHandler(support_t *currSupStructPTR)
{
    //Switch case in base al contenuto del reg_a0
    //
        //! Incrementiamo il pc, da inserire nelle syscall
        //! currSupStructPTR->sup_exceptState[GENERALEXCEPT].pc_epc += 4;
    switch (SUP_REG_A0)
    {
    case GETTOD:
        getTod(currSupStructPTR);
        break;

    case TERMINATE:
        terminate(currSupStructPTR);
        break;

    case WRITEPRINTER:
        writeToPrinter(currSupStructPTR, SUP_REG_A1, SUP_REG_A2);
        break;

    case WRITETERMINAL:
        writeToTerminal(currSupStructPTR, SUP_REG_A1, SUP_REG_A2);
        break;

    case READTERMINAL:
        readFromTerminal(currSupStructPTR, SUP_REG_A1);
        break;
    default:
        // TODO: termina il processo, forse la TERMINATE e' la default
        
    }
    // Carica lo stato di chi ha causato l'eccezione
    LDST(&(currSupStructPTR->sup_exceptState[GENERALEXCEPT]));
}

// Program Trap Exception Handler - Sezione 4.8
void trapExcHandler(support_t *currSupStructPTR)
{
    // todo: trovare il semaforo su cui è bloccato il processo; poi dovrebbe essere finito
    //? non capisco come cazzo accedere al semaforo; che faccio, scorro tutti i semafori possibili? sembra sbagliato come approccio
    /*
        if (current_proc semaphore == 0)
            SYSCALL(VERHOGEN, x, 0, 0)    //? dove x è indirizzo del semaforo su cui il processo è bloccato
        SYSCALL (TERMINATE, 0, 0, 0)
    */
}

#include "sysSupport.h"
// General exception handler - Sezione 4.6
void generalExcHandler()
{
    // Prende il current process supp struct
    support_t *currSupStruct = (support_t *)SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    /*determina la causa*/
    int cause = CAUSE_GET_EXCCODE(currSupStruct->sup_exceptState[GENERALEXCEPT].cause);

    // Se e' una syscall chiama il syscall exception handler
    if (cause == SYSEXCEPTION)
        syscallExcHandler(currSupStruct);

    // Se non è una syscall, passo la gestione al trap exception handler
    trapExcHandler(currSupStruct);
}
/* the Support Level’s SYSCALL exception handler must also increment
the PC by 4 in order to return control to the instruction after the
SYSCALL instruction. */
// !Chiedere ad andrea

// SYSCALL exception handler - Sezione 4.7
void syscallExcHandler(support_t *currSupStruct)
{
    // Prende la syscall
    int reg_a0 = currSupStruct->sup_exceptState[GENERALEXCEPT].reg_a0;
    int reg_a1 = currSupStruct->sup_exceptState[GENERALEXCEPT].reg_a1;
    int reg_a2 = currSupStruct->sup_exceptState[GENERALEXCEPT].reg_a2;


    switch (reg_a0)
    {
    case GETTOD:
        unsigned int retValue = SYSCALL(reg_a0, reg_a1, reg_a2, reg_a3);
        // Incrementiamo il pc
        currSupStruct->sup_exceptState[GENERALEXCEPT].pc_epc += 4;
        break;
    case TERMINATE:
        SYSCALL(TERMINATE, 0, 0, 0);
        terminate();
        // Incrementiamo il pc
        currSupStruct->sup_exceptState[GENERALEXCEPT].pc_epc += 4;
        break;
    case WRITEPRINTER:
        int retValue = SYSCALL(reg_a0, reg_a1, reg_a2, reg_a3);
        // Incrementiamo il pc
        currSupStruct->sup_exceptState[GENERALEXCEPT].pc_epc += 4;
        break;
    case WRITETERMINAL:
        int retValue = SYSCALL(reg_a0, reg_a1, reg_a2, reg_a3);
        // Incrementiamo il pc
        currSupStruct->sup_exceptState[GENERALEXCEPT].pc_epc += 4;
        break;
    case READTERMINAL:
        int retValue = SYSCALL(reg_a0, reg_a1, reg_a2, reg_a3);
        // Incrementiamo il pc
        currSupStruct->sup_exceptState[GENERALEXCEPT].pc_epc += 4;
        break;
    default:
        // TODO: termina il processo, forse la TERMINATE e' la default
        
    }
    // Carica lo stato di chi ha causato l'eccezione
    LDST(&(currSupStruct->sup_exceptState[GENERALEXCEPT]));
}

// Program Trap Exception Handler - Sezione 4.8
void trapExcHandler(support_t *currSupStruct)
{
    // todo: trovare il semaforo su cui è bloccato il processo; poi dovrebbe essere finito
    //? non capisco come cazzo accedere al semaforo; che faccio, scorro tutti i semafori possibili? sembra sbagliato come approccio
    /*
        if (current_proc semaphore == 0)
            SYSCALL(VERHOGEN, x, 0, 0)    //? dove x è indirizzo del semaforo su cui il processo è bloccato
        SYSCALL (TERMINATE, 0, 0, 0)
    */
}

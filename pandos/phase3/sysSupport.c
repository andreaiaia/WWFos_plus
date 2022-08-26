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

// SYSCALL exception handler - Sezione 4.7
void syscallExcHandler(support_t *currSupStruct)
{
    // Incrementiamo il pc
    currSupStruct->sup_exceptState[GENERALEXCEPT].pc_epc += 4;
    // Prende la syscall
    int sysNum = currSupStruct->sup_exceptState[GENERALEXCEPT].reg_a0;
    switch (sysNum)
    {
    case TERMINATE:
        SYSCALL(TERMINATE, 0, 0, 0);
        break;
    case GET_TOD:
        unsigned int retValue = SYSCALL(GETTOD, 0, 0, 0);
        break;
    case WRITEPRINTER:
        // TODO: ricavare virtAddr e len dai registri a1, a2
        int retValue = SYSCALL(WRITEPRINTER, char *virtAddr, int len, 0);
        break;
    case WRITETERMINAL:
        // TODO: ricavare virtAddr e len dai registri a1, a2
        int retValue = SYSCALL(WRITETERMINAL, char *virtAddr, int len, 0);
        break;
    case READTERMINAL:
        // TODO: ricavare virtAddr dal registro a1
        int retValue = SYSCALL(READTERMINAL, char *virtAddr, 0, 0);
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
    //* pseudocode
    /*
        if (current_proc semaphore == 0)
            SYSCALL(VERHOGEN, x, 0, 0)
        state_t->gpr[4] = 2   // o forse è tipo currSupStruct->sup_exceptState[GENERALEXCEPT].reg_a0 = 2
        SYSCALL (TERMINATE, 0, 0, 0)
    */
}

#include "sysSupport.h"
// General exception handler - Sezione 4.6
void generalExcHandler()
{
    // Prende il current process supp struct
    support_t *currSupStruct = (support_t *)SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    /*determina la causa*/
    int cause = CAUSE_GET_EXCCODE(currSupStruct->sup_exceptState[GENERALEXCEPT].cause);

    // Se e' una syscall la gestisce
    if (cause == SYSEXCEPTION)
    {
        // Incrementiamo il pc
        currSupStruct->sup_exceptState[GENERALEXCEPT].pc_epc += 4;
        // Prende la syscall
        int sysNum = currSupStruct->sup_exceptState[GENERALEXCEPT].reg_a0;
        switch (sysNum)
        {
        case TERMINATE:
            terminate();
            break;
        case GET_TOD:
            getTOD(currSupStruct);
            break;
        case WRITEPRINTER:
            writeToPrinter(currSupStruct);
            break;
        case WRITETERMINAL:
            writeToTerm(currSupStruct);
            break;
        case READTERMINAL:
            readFromTerm(currSupStruct);
            break;
        default:
            // TODO: termina il processo
        }
        // Carica lo stato di chi ha causato l'eccezione
        LDST(&(currSupStruct->sup_exceptState[GENERALEXCEPT]));
    }
    // TODO: se non e' una syscall termina il processo
}

// SYSCALL exception handler - Sezione 4.7

// Program Trap Exception Handler - Sezione 4.8
//? posso assumere che mi verrà passato il parametro in input? dovrebbe pensarci la generalExcHandler() a passarmelo

void trapExceptionHandler(support_t *currSupStruct)
{
    //! usare semaforo del support level non quello del livello 3
    //! mettere il valore 2 in a0
    //! la terminologia da usare è del tipo SYSCALL (TERMINATE, 0, 0, 0)

    /* PSEUDOCODE
    
        if (processo è in mutua excl)
            NSYS4
        a0 = 2
        SYSCALL (TERMINATE, 0, 0, 0)
    */

   if(){

   }
   currSupStruct->sup_exceptState[GENERALEXCEPT].reg_a0 = 2;
}

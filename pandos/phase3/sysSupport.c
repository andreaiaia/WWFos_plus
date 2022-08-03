#include "sysSupport.h"
// General exception handler - Sezione 4.6
void generalExcHandler()
{
    /*prende il current process supp struct*/
    support_t *currSupStruct = (support_t *)SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    /*determina la causa*/
    int cause = CAUSE_GET_EXCCODE(currSupStruct->sup_exceptState[GENERALEXCEPT].cause);

    /*se e' una syscall la gestisce*/
    if (cause == SYSEXCEPTION)
    {
        /*incrementiamo il pc*/
        currSupStruct->sup_exceptState[GENERALEXCEPT].pc_epc += 4;
        /*prende la syscall*/
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
            /* TODO termina il processo*/
        }
        /*carica lo stato di chi ha causato l'eccezione*/
        LDST(&(currSupStruct->sup_exceptState[GENERALEXCEPT]));
    }
    /* TODO se non e' una syscall termina il processo*/
}

// SYSCALL exception handler - Sezione 4.7

// Program Trap Exception Handler - Sezione 4.8
// ! Ti prego Alex ricordati di mettere le definizioni di funzione anche nel file .h
void trapExceptionHandler() //? ho messo void ma non ne sono certo, non vedo cosa dovrebbe tornare in ogni caso visto che deve solo UCCIDERE
{
    //? docs: 4.8

    // se processo in mutua esclusione prima libero il semaforo
    //! usare semaforo del support level non quello del livello 3
    //! mettere il valore 2 in a0
    //! la terminologia da usare è del tipo SYSCALL (TERMINATE, 0, 0, 0)
    if (semaforoLvlSupport != 0)
    { //? non sono affatto sicuro di come controllare se è in mutua esclusione
        Verhogen(current_p->semAdd);
        Terminate_Process(&(current_p->p_pid));
    }
    else
    {
        Terminate_Process(&(current_p->p_pid));
    }
    //? alla fine va chiamato scheduler()?
}

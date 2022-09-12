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
        SYSCALL(TERMPROCESS, 0 ,0 ,0);
    }
    // Carica lo stato di chi ha causato l'eccezione
    LDST(&(currSupStructPTR->sup_exceptState[GENERALEXCEPT]));
}


//* INIZIO SYSCALL 

void getTod(support_t *currSupStructPTR) {
    unsigned int valore_tod;
    STCK(valore_tod); 
    SUP_REG_V0 = valore_tod;
    INC_PC;
}

void terminate(support_t *currSupStructPTR){
    SYSCALL(TERMPROCESS, 0, 0, 0);
    INC_PC;
}

void writeToPrinter(support_t *currSupStructPTR, char *virtAddrPTR, int len) {
    
    //Ricaviamo il device ID facendo asid-1 (perchè gli asid contano da 1)
    int device_id = currSupStructPTR->sup_asid - 1;
    //Ricaviamo un puntatore a device (di tipo stampante) utilizzando la macro già definita in arch.h
    dtpreg_t *device = (dtpreg_t *)DEV_REG_ADDR(IL_PRINTER, device_id); 
    //Blocco il device selezionato sul relativo semaforo
    SYSCALL(PASSEREN, printer_sem[device_id], 0, 0);
    
    for (int i=0; i < len; i++) {
        //Indichiamo il punto dove iniziare a leggere/scrivere
        device->data0 = virtAddrPTR;
        int print_result = SYSCALL(DOIO, (int*)(device->command), TRANSMITCHAR, 0);
        //Controllo nel caso la print non vada a buon fine per illecito da parte del chiamante
        if (print_result != READY) trapExcHandler(currSupStructPTR);
    }
    INC_PC;
    SYSCALL(VERHOGEN, printer_sem[device_id], 0, 0);
}

int writeToTerminal(support_t *currSupStructPTR, char *virtAddrPTR, int len) {
   
     //Ricaviamo il device ID facendo asid-1 (perchè gli asid contano da 1)
    int device_id = currSupStructPTR->sup_asid - 1;
    //Ricaviamo un puntatore a device (di tipo stampante) utilizzando la macro già definita in arch.h
    termreg_t *device = (termreg_t *)DEV_REG_ADDR(IL_TERMINAL, device_id); 
    //Blocco il device selezionato sul relativo semaforo
    SYSCALL(PASSEREN, term_w_sem[device_id], 0, 0);
    
    for (int i=0; i < len; i++) {
        int valore = (TRANSMITCHAR | (unsigned int)SUP_REG_A1 << 8); //capire perché
        //Indichiamo il punto dove iniziare a leggere/scrivere
        int print_result = SYSCALL(DOIO, (int*)(device->transm_command), valore, 0);
        //Controllo nel caso la print non vada a buon fine per illecito da parte del chiamante
        if (print_result != READY) trapExcHandler(currSupStructPTR);
    }
    INC_PC;
    SYSCALL(VERHOGEN, term_w_sem[device_id], 0, 0);
}

int readFromTerminal(support_t *currSupStructPTR, char *virtAddrPTR) {
    //!DA FARE
     //Ricaviamo il device ID facendo asid-1 (perchè gli asid contano da 1)
    int device_id = currSupStructPTR->sup_asid - 1;
    //Ricaviamo un puntatore a device (di tipo stampante) utilizzando la macro già definita in arch.h
    termreg_t *device = (termreg_t *)DEV_REG_ADDR(IL_TERMINAL, device_id); 
    //Blocco il device selezionato sul relativo semaforo
    SYSCALL(PASSEREN, term_r_sem[device_id], 0, 0);
    char guard = '\0';
    while (guard != '\n') {
        unsigned int res = SYSCALL(DOIO, (int*)(device->recv_command), 2, 0); //occhio al tipo passato come secondo parametro (il cast)


    }

    for (int i=0; i < len; i++) {
        //Indichiamo il punto dove iniziare a leggere/scrivere
        device->data0 = virtAddrPTR;
        int print_result = SYSCALL(DOIO, (int*)device->command, TRANSMITCHAR, 0);
        //Controllo nel caso la print non vada a buon fine per illecito da parte del chiamante
        if (print_result != READY) trapExcHandler(currSupStructPTR);
    }
    INC_PC;
    SYSCALL(VERHOGEN, term_r_sem[device_id], 0, 0);
}



// commento la parte di alex perché è maledetta e non sa scrive codice
// // Program Trap Exception Handler - Sezione 4.8
// void trapExcHandler(support_t *currSupStructPTR)
// {
//     // todo: trovare il semaforo su cui è bloccato il processo; poi dovrebbe essere finito
//     //? non capisco come cazzo accedere al semaforo; che faccio, scorro tutti i semafori possibili? sembra sbagliato come approccio
//     /*
//         if (current_proc semaphore == 0)
//             SYSCALL(VERHOGEN, x, 0, 0)    //? dove x è indirizzo del semaforo su cui il processo è bloccato
//         SYSCALL (TERMINATE, 0, 0, 0)
//     */
// }

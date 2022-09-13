#include "sysSupport.h"

// General exception handler - Sezione 4.6
void generalExcHandler()
{
    // Prende il current process supp struct
    support_t *currSupStructPTR = (support_t *)SYSCALL(GETSUPPORTPTR, 0, 0, 0);
    // Determina la causa
    int cause = CAUSE_GET_EXCCODE(currSupStructPTR->sup_exceptState[GENERALEXCEPT].cause);

    // Se è una syscall chiama il syscall exception handler
    if (cause == SYSEXCEPTION)
        syscallExcHandler(currSupStructPTR);

    // Se non è una syscall, passo la gestione al trap exception handler
    trapExcHandler(currSupStructPTR);
}

// SYSCALL exception handler - Sezione 4.7
void syscallExcHandler(support_t *currSupStructPTR)
{
    // Switch case in base al contenuto del reg_a0
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
        trapExcHandler(currSupStructPTR); // sperando che killi
    }
    // Carica lo stato di chi ha causato l'eccezione
    LDST(&(currSupStructPTR->sup_exceptState[GENERALEXCEPT]));
}

// Program Trap Exception Handler - Sezione 4.8
void trapExcHandler(support_t *currSupStructPTR)
{
    int asid = currSupStructPTR->sup_asid;
    for (int i = 0; i < POOLSIZE; i++)
    {
        if (swap_pool_table[i].sw_asid == asid)
            swap_pool_table[i].sw_asid = -1;
    }
    // Sblocco il traffico
    SYSCALL(VERHOGEN, (int)&swapSemaphore, 0, 0);
    SYSCALL(VERHOGEN, (int)&mainSemaphore, 0, 0);
    // Ammazzo il processo
    SYSCALL(TERMINATE, 0, 0, 0);
}

//* INIZIO SYSCALLS

void getTod(support_t *currSupStructPTR)
{
    unsigned int valore_tod;
    STCK(valore_tod);
    SUP_REG_V0 = valore_tod;
    INC_PC;
}

void terminate(support_t *currSupStructPTR)
{
    SYSCALL(TERMPROCESS, 0, 0, 0);
    INC_PC;
}

void writeToPrinter(support_t *currSupStructPTR, char *virtAddrPTR, int len)
{
    if (SUP_REG_A1 < KUSEG || len < 0 || len > 128)
        trapExcHandler(currSupStructPTR);
    // Ricaviamo il device ID facendo asid-1 (perchè gli asid contano da 1)
    int device_id = currSupStructPTR->sup_asid - 1;
    // Ricaviamo un puntatore a device (di tipo stampante) utilizzando la macro già definita in arch.h
    dtpreg_t *device = (dtpreg_t *)DEV_REG_ADDR(IL_PRINTER, device_id);
    // Blocco il device selezionato sul relativo semaforo
    SYSCALL(PASSEREN, printer_sem[device_id], 0, 0);

    for (int i = 0; i < len; i++)
    {
        // Indichiamo il punto dove iniziare a leggere/scrivere
        device->data0 = virtAddrPTR;
        int res = SYSCALL(DOIO, (int *)(device->command), TRANSMITCHAR, 0);
        // Controllo nel caso la print non vada a buon fine per illecito da parte del chiamante
        if (res != READY)
        {
            SUP_REG_V0 = -(res & 0xF);
            SYSCALL(VERHOGEN, term_r_sem[device_id], 0, 0);
            return;
        }
    }
    INC_PC;
    SYSCALL(VERHOGEN, printer_sem[device_id], 0, 0);
    SUP_REG_V0 = len;
}

void writeToTerminal(support_t *currSupStructPTR, char *virtAddrPTR, int len)
{
    if (SUP_REG_A1 < KUSEG || len < 0 || len > 128)
        trapExcHandler(currSupStructPTR);

    // Ricaviamo il device ID facendo asid-1 (perchè gli asid contano da 1)
    int device_id = currSupStructPTR->sup_asid - 1;
    // Ricaviamo un puntatore a device (di tipo stampante) utilizzando la macro già definita in arch.h
    termreg_t *device = (termreg_t *)DEV_REG_ADDR(IL_TERMINAL, device_id);
    // Blocco il device selezionato sul relativo semaforo
    SYSCALL(PASSEREN, term_w_sem[device_id], 0, 0);
    for (int i = 0; i < len; i++)
    {
        int valore = (TRANSMITCHAR | (unsigned int)SUP_REG_A1 << BYTELENGTH); // capire perché
        // Indichiamo il punto dove iniziare a leggere/scrivere
        int res = SYSCALL(DOIO, (int *)(device->transm_command), valore, 0);
        // Controllo nel caso la print non vada a buon fine per illecito da parte del chiamante
        if (res != OKCHARTRANS)
        {
            SUP_REG_V0 = -(res & 0xF);
            SYSCALL(VERHOGEN, term_r_sem[device_id], 0, 0);
            return;
        }
    }
    INC_PC;
    SYSCALL(VERHOGEN, term_w_sem[device_id], 0, 0);
    SUP_REG_V0 = len;
}

void readFromTerminal(support_t *currSupStructPTR, char *virtAddrPTR)
{
    /**
     * Come da specifiche, leggere da un dispositivo terminale al
     * di fuori del proprio u-proc logical address space è un errore
     * e il processo va terminato.
     */
    if (SUP_REG_A1 < KUSEG)
        trapExcHandler(currSupStructPTR);

    char buffer[MAXSTRLENG];

    // Ricaviamo il device ID facendo asid-1 (perchè gli asid contano da 1)
    int device_id = currSupStructPTR->sup_asid - 1;
    // Ricaviamo un puntatore a device (di tipo stampante) (macro di arch.h)
    termreg_t *device = (termreg_t *)DEV_REG_ADDR(IL_TERMINAL, device_id);
    // Blocco il device selezionato sul relativo semaforo
    SYSCALL(PASSEREN, term_r_sem[device_id], 0, 0);
    int i = 0;
    char guard = '\0';
    while (guard != '\n' && i < MAXSTRLENG)
    {
        // occhio al tipo passato come secondo parametro (il cast)
        unsigned int res = SYSCALL(DOIO, (int *)(device->recv_command), TRANSMITCHAR, 0);
        if (res != OKCHARTRANS)
        {
            SUP_REG_V0 = -(res & 0xF);
            SYSCALL(VERHOGEN, term_r_sem[device_id], 0, 0);
            return;
        }
        /**
         * Caso in cui lettura andata a buon fine shiftiamo di 8 bit
         * e facciamo l'or bit a bit con 0xFF (11111111) [5.7 pops]
         */
        buffer[i], guard = (res >> 8) & 0xFF;
        i++;
    }
    INC_PC;
    SYSCALL(VERHOGEN, term_r_sem[device_id], 0, 0);
    buffer[i] = '\0';
    SUP_REG_V0 = i;
}

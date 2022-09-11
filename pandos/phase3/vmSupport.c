#include "vmSupport.h"

/**
 * Questo modulo implementa il TLB exception handler
 * e le funzioni per leggere e scrivere i dispositivi flash
 * implementa anche Swap Pool e Swap pool table e una funzione
 * initSwapStructs da richiamare poi in initProc.c
 */

// Swap pool table e relative strutture dati
swap_t swap_pool_table[POOLSIZE];
int swap_semaphore;

void TLB_ExcHandler()
{
    /**
     * Innanzitutto recupero il puntatore alla struttura di
     * supporto del processo che ha sollevato la TLB exception
     */
    support_t *currSupStruct = SYSCALL(GETSUPPORTPTR, 0, 0, 0);

    // Determino la causa della TLB Exception
    int cause = CAUSE_GET_EXCCODE(currSupStruct->sup_exceptState[PGFAULTEXCEPT].cause);

    /**
     * Se l'eccezione è di tipo TLB_modification la
     * tratto come una trap, altrimenti proseguo nella gestione
     */
    if (cause == TLB_MODIFICATION)
    {
        // Attempt to write on a read-only page
        trapExcHandler(currSupStruct);
    }
    else
    {
        // Prendo l'accesso alla swap pool table
        SYSCALL(PASSEREN, (int)&swap_semaphore, 0, 0);

        // Trova la missing page number (indicata con p) dal processor saved state
        state_t *procSavedState = &currSupStruct->sup_exceptState[PGFAULTEXCEPT];
        size_tt p = getPTEIndex(procSavedState->entry_hi);

        // Prendo un frame i dallo swap pool usando l'algoritmo di pandos
        int i = pandosPageReplacementAlgorithm();
        swap_t swap_frame = swap_pool_table[i];

        // Controllo se il frame scelto è occupato
        if (swap_frame.sw_asid != NOPROC)
        {
            // Queste operazioni vanno fatte in modo atomico
            // Quindi disattivo gli interrupts
            setSTATUS(getSTATUS() & DISABLEINTS);

            // Segno la pagina come invalida
            currSupStruct->sup_privatePgTbl[i].pte_entryLO &= !VALIDON;

            // Aggiorno il TLB
            TLB_updater(currSupStruct->sup_privatePgTbl[p]);

            /**
             * Scrivo il contenuto da swap_frame al
             * backing store del processo. Per farlo devo:
             *
             ** 1. Scrivere nel campo DATA0 del flash device
             **    l'indirizzo fisico dell'inizio del blocco da scrivere.
             */
            dtpreg_t *dev = (dtpreg_t *)DEV_REG_ADDR(FLASHINT, swap_frame.sw_asid - 1);
            dev->data0 = (memaddr)&swap_frame;

            //* 2. Effettuare la scrittura con la NSYS5
            int write_result = SYSCALL(DOIO, (int)&dev->command, FLASHWRITE, 0);

            // Qualsiasi errore viene gestito come una trap
            if (write_result != READY)
                trapExcHandler(currSupStruct);

            // Adesso posso riattivare gli interrupts
            setSTATUS(getSTATUS() | IECON);
        }
        /**
         * Se il frame è libero, leggo il backing store
         * del processo corrente dalla pagina p al swap_frame
         */
        dtpreg_t *dev = (dtpreg_t *)DEV_REG_ADDR(FLASHINT, currSupStruct->sup_asid - 1);
        dev->data0 = (memaddr)&swap_frame;
        int read_result = SYSCALL(DOIO, (int)&dev->command, FLASHREAD, 0);

        // Qualsiasi errore viene gestito come una trap
        if (read_result != READY)
            trapExcHandler(currSupStruct);

        // Aggiorno la swap_pool_table con i nuovi contenuti del frame i
        swap_pool_table[i].sw_asid = currSupStruct->sup_asid;
        swap_pool_table[i].sw_pageNo = procSavedState->entry_hi >> VPNSHIFT;
        swap_pool_table[i].sw_pte = currSupStruct->sup_privatePgTbl + p;

        // Queste operazioni vanno fatte in modo atomico
        // Quindi disattivo gli interrupts
        setSTATUS(getSTATUS() & DISABLEINTS);

        // Aggiorno la page table entry del current process
        currSupStruct->sup_privatePgTbl[p].pte_entryLO = (memaddr)swap_frame | VALIDON | DIRTYON;

        // Aggiorno il TLB
        TLB_updater(currSupStruct->sup_privatePgTbl[p]);

        // Tana libera tutti
        SYSCALL(VERHOGEN, (int)&swap_semaphore, 0, 0);

        // Adesso posso riattivare gli interrupts
        setSTATUS(getSTATUS() | IECON);

        // Restituisco il controllo al processo che ha sollevato la TLB exc
        LDST(procSavedState);
    }
}

/**
 * Funzione per consentire al test in initProc di inizializzare
 * le strutture dati
 */
void initSwapStructs()
{
    // Inizializzo il semaforo della swap pool table
    swap_semaphore = 1;
    // Faccio una P sul semaforo, dal momento che devo accedere alla swap pool table
    SYSCALL(PASSEREN, (int)&swap_semaphore, 0, 0);
    /**
     * Segno come non allocate tutte le entry della swap pool.
     * Per farlo uso ASID -1 come suggerito nel libro, dal
     * momento che gli ASID sono positivi.
     */
    for (int i = 0; i < UPROCMAX; i++)
        swap_pool_table[i].sw_asid = -1;

    // Faccio la V per liberare il semaforo
    SYSCALL(VERHOGEN, (int)&swap_semaphore, 0, 0);
}

/**
 * The recommended Pandos page replacement algorithm is First in First out.
 * This algorithm is easily implemented via a static variable.
 * Whenever a frame is needed to support a page fault, simply
 * increment this variable mod the size of the Swap Pool.
 */
int pandosPageReplacementAlgorithm()
{
    // Prima cerco un frame libero alla maniera old style
    for (int j = 0; j < POOLSIZE; ++j)
    {
        if ((swap_pool_table[j].sw_asid == NOPROC))
            return j;
    }
    // Se non trovo nulla, procedo con l'algoritmo di pandos
    static int i = -1;
    i = (i + 1) % POOLSIZE;
    return i;
}

void TLB_updater(pteEntry_t pteEntry)
{
    setENTRYHI(pteEntry.pte_entryHI);
    TLBP();
    if (getINDEX() & PRESENTFLAG)
    {
        setENTRYHI(pteEntry.pte_entryHI);
        setENTRYLO(pteEntry.pte_entryLO);
        TLBWI();
    }
}
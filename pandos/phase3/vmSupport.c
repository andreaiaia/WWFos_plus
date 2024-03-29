#include "vmSupport.h"

/**
 * Questo modulo implementa il TLB exception handler
 * e le funzioni per leggere e scrivere i dispositivi flash
 * implementa anche Swap Pool e Swap pool table e una funzione
 * initSwapStructs da richiamare poi in initProc.c
 */
// Swap pool table e relative strutture dati
swap_t swap_pool_table[POOLSIZE];
int swapSemaphore;

extern void klog_print_hex();

void TLB_ExcHandler()
{
    klog_print("Sono nell'exc tlb handler\n");
    /**
     * Innanzitutto recupero il puntatore alla struttura di
     * supporto del processo che ha sollevato la TLB exception
     */
    support_t *currSupStructPTR = (support_t *)SYSCALL(GETSUPPORTPTR, 0, 0, 0);

    // Determino la causa della TLB Exception
    int cause = CAUSE_GET_EXCCODE(currSupStructPTR->sup_exceptState[PGFAULTEXCEPT].cause);

    /**
     * Se l'eccezione è di tipo TLB_modification la
     * tratto come una trap, altrimenti proseguo nella gestione
     */
    if (cause == TLB_MODIFICATION)
    {
        klog_print("chiamo una trap\n");
        // Attempt to write on a read-only page
        trapExcHandler();
    }
    else
    {
        klog_print("chiamo una passeren\n");
        // Prendo l'accesso alla swap pool table
        SYSCALL(PASSEREN, (int)&swapSemaphore, 0, 0);

        // Trova la missing page number (indicata con p) dal processor saved state
        state_t *procSavedState = &currSupStructPTR->sup_exceptState[PGFAULTEXCEPT];
        //int index = ENTRYHI_GET_ASID(procSavedState->entry_hi);
        int asid = ENTRYHI_GET_ASID(procSavedState->entry_hi);
        klog_print("refill handler\n");
        unsigned int vpn = procSavedState->entry_hi >> VPNSHIFT;

        pteEntry_t *table = currSupStructPTR->sup_privatePgTbl;
        unsigned int newEntryHI, newEntryLO;
        for (int i = 0; i < USERPGTBLSIZE; i++)
        {
            if (table[i].pte_entryHI >> VPNSHIFT == vpn)
            {
                newEntryHI = table[i].pte_entryHI;
                newEntryLO = table[i].pte_entryLO;
                klog_print("Trovata pagina: ");
                klog_print_hex(i);
                klog_print("vmsupport\n");
                break;
            }
        }


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
            currSupStructPTR->sup_privatePgTbl[i].pte_entryLO &= !VALIDON;

            // Aggiorno il TLB
            TLB_updater(newEntryHI, newEntryLO);

            /**
             * Scrivo il contenuto da swap_frame al
             * backing store del processo. Per farlo devo:
             *
             ** 1. Scrivere nel campo DATA0 del flash device
             **    l'indirizzo fisico dell'inizio del blocco da scrivere.
             */
            dtpreg_t *dev = (dtpreg_t *)DEV_REG_ADDR(FLASHINT, swap_frame.sw_asid - 1);
            dev->data0 = (memaddr)&swap_frame;

            //* 2. Effettuare la scrittura con la DOIO (NSYS5)
            int write_result = SYSCALL(DOIO, (int)&dev->command, FLASHWRITE | asid << BYTELENGTH, 0);

            // Qualsiasi errore viene gestito come una trap
            if (write_result != READY){
                trapExcHandler();
                
            klog_print("chiamo una trap 2\n");
            }
            // Adesso posso riattivare gli interrupts
            setSTATUS(getSTATUS() | IECON);
        }
        /**
         * Se il frame è libero, leggo il backing store
         * del processo corrente dalla pagina p al swap_frame
         */
        dtpreg_t *dev = (dtpreg_t *)DEV_REG_ADDR(FLASHINT, currSupStructPTR->sup_asid - 1);
        dev->data0 = (memaddr)&swap_frame;
        int read_result = SYSCALL(DOIO, (int)&dev->command, FLASHREAD, 0);

        // Qualsiasi errore viene gestito come una trap
        if (read_result != READY)
            trapExcHandler();

        // Aggiorno la swap_pool_table con i nuovi contenuti del frame i
        swap_pool_table[i].sw_asid = currSupStructPTR->sup_asid;
        swap_pool_table[i].sw_pageNo = ENTRYHI_GET_VPN(procSavedState->entry_hi);
        swap_pool_table[i].sw_pte = currSupStructPTR->sup_privatePgTbl + asid;

        // Queste operazioni vanno fatte in modo atomico
        // Quindi disattivo gli interrupts
        setSTATUS(getSTATUS() & DISABLEINTS);

        // Aggiorno la page table entry del current process
        memaddr swap_frame_addr = SWAPSTART + i * PAGESIZE;
        currSupStructPTR->sup_privatePgTbl[asid].pte_entryLO = swap_frame_addr | VALIDON | DIRTYON;

        // Aggiorno il TLB
        TLB_updater(newEntryHI, newEntryLO);

        // Tana libera tutti
        SYSCALL(VERHOGEN, (int)&swapSemaphore, 0, 0);

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
    swapSemaphore = 1;
    // Faccio una P sul semaforo, dal momento che devo accedere alla swap pool table
    SYSCALL(PASSEREN, (int)&swapSemaphore, 0, 0);
    /**
     * Segno come non allocate tutte le entry della swap pool.
     * Per farlo uso ASID -1 come suggerito nel libro, dal
     * momento che gli ASID sono positivi.
     */
    for (int i = 0; i < UPROCMAX; i++)
        swap_pool_table[i].sw_asid = -1;

    // Faccio la V per liberare il semaforo
    SYSCALL(VERHOGEN, (int)&swapSemaphore, 0, 0);
}

/**
 * L'algoritmo raccomandato da pandos è di tipo FIFO.
 * È facilmente implementabile con una variabile statica.
 * Quando un frame deve supportare un page fault, si
 * incrementa questa variabile e si fa il modulo con la
 * dimensione della swap pool.
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

void TLB_updater(unsigned int newEntryHI, unsigned int newEntryLO)
{
        setENTRYHI(newEntryHI);
        setENTRYLO(newEntryLO);
        TLBWI();
}
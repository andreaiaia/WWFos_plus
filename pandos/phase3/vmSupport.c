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
    support_t *guiltySupportStructure = SYSCALL(GETSUPPORTPTR, 0, 0, 0);

    // Determino la causa della TLB Exception
    int cause = CAUSE_GET_EXCCODE(currSupStruct->sup_exceptState[PGFAULTEXCEPT].cause);

    /**
     * Se l'eccezione è di tipo TLB_modification la
     * tratto come una trap, altrimenti proseguo nella gestione
     */
    if (cause == TLB_MODIFICATION)
    {
        // Attempt to write on a read-only page
        trapExcHandler(guiltySupportStructure);
    }
    else
    {
        // Prendo l'accesso alla swap pool table
        // ! RICORDA DI FARE LE V DOVE SERVONO DOPO
        SYSCALL(PASSEREN, (int)&swap_semaphore, 0, 0);

        // Trova la missing page number (indicata con p) dal processor saved state
        state_t *procSavedState = &guiltySupportStructure->sup_except_state[PGFAULTEXCEPT];
        size_t p = getPTEIndex(procSavedState->entry_hi);

        // Prendo un frame i dallo swap pool usando l'algoritmo di pandos
        int i = pandosPageReplacementAlgorithm;
        swap_t swap_frame = swap_pool_table[];

        // Controllo se il frame scelto è occupato
        if (swap_frame.sw_asid != NOPROC)
        {
            // Queste operazioni vanno fatte in modo atomico
            // Quindi disattivo gli interrupts
            setSTATUS(getSTATUS() & DISABLEINTS);

            // Segno la pagina come invalida
            (guiltySupportStructure->sup_privatePgTbl[i])
                ->pte_entryLO &= !VALIDON;

            // Aggiorno il TLB
            setENTRYHI(*swap_frame.sw_pte->pte_entryHI);
            setENTRYLO(*swap_frame.sw_pte->pte_entryLO);
            TLBWI();

            // Adesso posso riattivare gli interrupts
            setSTATUS(getSTATUS() | IECON);
        }

        //* IT'S A WRAP
        // Continua da paragrafo [4.5.1]
        // (poi torna al punto 8c del paragrafo [4.4.2])

        // if (cause == TLB_INVALID_LOAD)
        // {
        //     // Page fault on load operation
        // }
        // else if (cause == TLB_INVALID_STORE)
        // {
        //     // Page fault on store operation
        // }

        SYSCALL(VERHOGEN, (int)&swap_semaphore, 0, 0);
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
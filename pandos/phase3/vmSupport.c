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
    memaddr guiltySupportStructure = SYSCALL(GETSUPPORTPTR, 0, 0, 0);

    // Determino la causa della TLB Exception
    int cause = CAUSE_GET_EXCCODE(currSupStruct->sup_exceptState[PGFAULTEXCEPT].cause);

    if (cause == TLB_INVALID_LOAD)
    {
        // Page fault on load operation
    }
    else if (cause == TLB_INVALID_STORE)
    {
        // Page fault on store operation
    }
    else if (cause == TLB_MODIFICATION)
    {
        // Attempt to write on a read-only page
        trapExcHandler(guiltySupportStructure);
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
#include "vmSupport.h"

/**
 * Questo modulo implementa il TLB exception handler
 * e le funzioni per leggere e scrivere i dispositivi flash
 * implementa anche Swap Pool e Swap pool table e una funzione
 * initSwapStructs da richiamare poi in initProc.c
 */

/**
 * Il TLB-Refill handler è l'unica parte del livello supporto
 * ad avere accesso alle strutture dati di livello Kernel, quindi il
 * current_p può essere usato solo da lui.
 */

extern pcb_PTR current_p;

// Swap pool table e relative strutture dati
swap_t swap_pool_table[POOLSIZE];
int swap_semaphore;

void uTLB_RefillHandler()
{
    state_t *procSavedState = (state_t *)BIOSDATAPAGE;
    // ---------------------
    size_t index = getPTEIndex(procSavedState->entry_hi);
    pteEntry_t pte = current_p->p_supportStruct->sup_privatePgTbl[index];

    // Aggiungo la PTE nel TLB
    setENTRYHI(pte.pte_entryHI);
    setENTRYLO(pte.pte_entryLO);
    TLBWR();

    LDST(procSavedState);
}

size_t getPTEIndex(memaddr entry_hi)
{
    // processo: prendi EntryHI e ricavi il VPN, poi dal VPN ricavi l'index
    size_t vpn = entryhi >> VPNSHIFT;
    size_t index = vpn - (KUSEG >> VPNSHIFT);

    return index;
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
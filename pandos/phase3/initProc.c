// Questo file implementa il test e esporta le
// variabili globali del livello support (sezioni 4.9 e 4.10)

#include "initProc.h"

#include "../h/pandos_types.h"
#include "../h/pandos_const.h"

// Strutture di supporto dei processi
support_t support_table[UPROCMAX];
list_head support_free;

// Swap pool table e relative strutture dati
swap_t swap_pool_table[POOLSIZE];
int swap_semaphore;

int mainSemaphore;

void test_fase3()
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

    // Fine del test, chiudiamo baracca e burattini
    HALT();
}
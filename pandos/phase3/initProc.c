// Questo file implementa il test e esporta le
// variabili globali del livello support (sezioni 4.9 e 4.10)

#include "initProc.h"

#include "../h/pandos_types.h"
#include "../h/pandos_const.h"

// Strutture di supporto dei processi
support_t support_table[UPROCMAX];
list_head support_free;

// Semafori per tutti i dispositivi
int device_sem[DEVSEM_NUM];

int mainSemaphore;

void test_fase3()
{
    // Inizializzo le strutture dati della Swap Pool Table
    initSwapStructs();

    // Inizializzo il semaforo principale
    mainSemaphore = 0;

    // Fine del test, chiudiamo baracca e burattini
    HALT();
}
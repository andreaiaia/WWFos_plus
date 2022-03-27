#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/asl.c"
#include "scheduler.h"

/* Costanti */
#define SEM_NUM 49;

/* Variabili Globali */
// Process Count - Contatore processi vivi (started but not yet finished)
int proc_count;
// Soft-Block Count - Contatore dei processi avviati ma non ancora terminati (e quindi bloccati)
int soft_count;
// Ready Queue - Puntatore a Tail della coda dei pcb che sono in stato "Ready"
list_head *ready_q;
// Current Process - Puntatore a pcb in stato "Running" (correntemente attivo)
pcb_PTR current_p;
// Device Semaphores - we need 49 sem in total
int device_sem[SEM_NUM];

// TODO: Inizializzare il kernel
int main()
{
    // Inizializzo le variabili globali e inizializzo la coda dei processi
    initPcbs();
    initASL();
    proc_count = 0;
    soft_count = 0;
    mkEmptyProcQ(ready_q);
    current_p = NULL;
    for (int i = 0; i < SEM_NUM; i++)
    {
        device_sem[i] = 0;
    }

    return 0;
}

#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/asl.c"
#include "scheduler.h"

/* Variabili Globali */
// Process Count - Contatore processi vivi (started but not yet finished)
int proc_count = 0;
// Soft-Block Count - Contatore dei processi avviati ma non ancora terminati (e quindi bloccati)
int soft_count = 0;
// Ready Queue - Puntatore a Tail della coda dei pcb che sono in stato "Ready"
pcb_PTR ready_q;
// Current Process - Puntatore a pcb in stato "Running" (correntemente attivo)
pcb_PTR current_p;
// Device Semaphores - we need 49 sem in total
int device_sem[49]; // forse è meglio non hard codarlo, vedrem

// TODO: Inizializzare il kernel

int main()
{

    return 0;
}
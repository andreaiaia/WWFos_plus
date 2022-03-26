#include "../phase1/asl.h"
#include "../phase1/asl.c"

// Variabili Globali
/*
    - Puntatore a processo correntemente attivo
    - Un semaforo (una variabile int) per ogni (sub) dispositivo, non sono tutti sempre attivi insieme
    - Strutture dati già create in fase 1
*/

// Process Count - Contatore processi vivi (started but not yet finished)
#define ProcCount 0
// Soft-Block Count - Contatore dei processi avviati ma non ancora terminati (e quindi bloccati)
#define SoftCount 0
// Ready Queue - Puntatore a Tail della coda dei pcb che sono in stato "Ready"
#define pcb_t *ReadyQ
// Current Process - Puntatore a pcb in stato "Running" (correntemente attivo)
#define pcb_t *Current
// Device Semaphores -

// TODO: Inizializzare il kernel (Usando initpcb e initsemd)

int main()
{

    return 0;
}
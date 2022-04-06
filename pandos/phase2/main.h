#ifndef MAIN_H
#define MAIN_H

#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"
#include "scheduler.h"
#include "exception.h"

//* Costanti */
// Numero di semafori dei dispositivi
#define DEVSEM_NUM 49

//* Variabili Globali */

// Process Count - Contatore processi vivi (started but not yet finished)
int proc_count;
// Soft-Block Count - Contatore dei processi avviati ma non ancora terminati (e quindi bloccati)
int soft_count;

// tail-pointer dei processi in "ready"; alta e bassa priorità
// (Le code dei processi ready sono dichiarate in scheduler.h)
extern struct list_head *high_ready_q, *low_ready_q;

// Current Process - Puntatore a pcb in stato "Running" (correntemente attivo)
pcb_PTR current_p;
// Device Semaphores - we need 49 sem in total
// Ultimo semaforo è il pseudo-clock semaphore
int device_sem[DEVSEM_NUM];
// Passup Vector
passupvector_t *pu_vector;

//* Dichiarazioni di funzioni esterne */

// Funzione fornita dalle specifiche, la riscriveremo nella prossima fase
extern void uTLB_RefillHandler();
// La funzione test si trova nel file di test fornito
extern void test();

#endif
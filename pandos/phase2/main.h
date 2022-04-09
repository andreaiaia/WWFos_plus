#ifndef MAIN_H
#define MAIN_H

#include "scheduler.h"

//* Variabili Globali */
// Richiamo le variabili globali dallo scheduler
extern int proc_count;
extern int soft_count;
extern pcb_PTR current_p;
extern int device_sem[DEVSEM_NUM];
extern passupvector_t *pu_vector;
// tail-pointer dei processi in "ready"; alta e bassa priorità
extern struct list_head *high_ready_q, *low_ready_q;

// Passup Vector
passupvector_t *pu_vector;

//* Dichiarazioni di funzioni esterne */
// Funzione fornita dalle specifiche, la riscriveremo nella prossima fase
extern void uTLB_RefillHandler();
// La funzione test si trova nel file di test fornito
extern void test();

#endif
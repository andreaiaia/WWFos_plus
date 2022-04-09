#ifndef MAIN_H
#define MAIN_H

#include "scheduler.h"

// Passup Vector
passupvector_t *pu_vector;

//* Dichiarazioni di funzioni esterne */
// Funzione fornita dalle specifiche, la riscriveremo nella prossima fase
extern void uTLB_RefillHandler();
// La funzione test si trova nel file di test fornito
extern void test();

#endif
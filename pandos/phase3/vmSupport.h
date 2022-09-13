#ifndef VM_SUPPORT_H
#define VM_SUPPORT_H

#include "pandos_types.h"
#include "pandos_const.h"
#include "helpersSupport.h"

// Renaming some umps const's written in black speech of mordor
#define TLB_INVALID_LOAD TLBINVLDL
#define TLB_INVALID_STORE TLBINVLDS
#define TLB_MODIFICATION 1

// Richiamo funzioni dagli altri moduli
extern void trapExcHandler(support_t *);

// Swap pool table e relative strutture dati
swap_t swap_pool_table[POOLSIZE];
int swapSemaphore;

void TLB_ExcHandler();

void initSwapStructs();

#endif
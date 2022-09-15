#ifndef VM_SUPPORT_H
#define VM_SUPPORT_H

#include "pandos_types.h"
#include "pandos_const.h"

#define SWAPSTART ((32 * PAGESIZE) + RAMSTART)
#define TLB_MODIFICATION 1

// Richiamo funzioni dagli altri moduli
extern void trapExcHandler();

// Swap pool table e relative strutture dati
swap_t swap_pool_table[POOLSIZE];
int swapSemaphore;

void TLB_ExcHandler();

void initSwapStructs();

#endif
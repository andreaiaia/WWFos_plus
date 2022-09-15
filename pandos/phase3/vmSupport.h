#ifndef VM_SUPPORT_H
#define VM_SUPPORT_H

#include "inclusive.h"
#include "sysSupport.h"

#define SWAPSTART ((32 * PAGESIZE) + RAMSTART)
#define TLB_MODIFICATION 1

int pandosPageReplacementAlgorithm();

void TLB_ExcHandler();
void TLB_updater(pteEntry_t pteEntry);
void initSwapStructs();

#endif
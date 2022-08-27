#ifndef VM_SUPPORT_H
#define VM_SUPPORT_H

#include "pandos_types.h"
#include "pandos_const.h"
#include "helpersSupport.h"

void uTLB_RefillHandler();

size_t getPTEIndex(memaddr);

void initSwapStructs();

#endif
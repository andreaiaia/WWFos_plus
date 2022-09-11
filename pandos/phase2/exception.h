#ifndef EXC_H
#define EXC_H
#include "initial.h"
#include "helpers.h"
#include "scheduler.h"
#include "interrupt.h"
#include "SYSCALL.h"

extern size_tt getPTEIndex(memaddr);

void exceptionHandler();
void uTLB_RefillHandler();

#endif

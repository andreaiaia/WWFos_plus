#ifndef EXC_H
#define EXC_H
#include "sistema.h"
#include "SYSCALL_helpers.h"
#include "scheduler.h"
#include "interrupt.h"
#include "SYSCALL.h"

void exceptionHandler();
extern void uTLB_RefillHandler();

#endif

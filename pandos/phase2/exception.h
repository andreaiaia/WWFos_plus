#ifndef EXC_H
#define EXC_H
#include "sistema.h"
#include "SYSCALL_helpers.h"
#include "scheduler.h"
#include "interrupt.h"
#include "SYSCALL.h"

void exceptionHandler();
extern void uTLB_RefillHandler();
#define incremento_pc current_p->p_s.pc_epc += WORDLEN
#define stato_processo ((state_t *)BIOSDATAPAGE)
void PassUpOrDie(int excCode);

#endif

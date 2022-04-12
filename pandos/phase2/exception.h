#ifndef EXC_H
#define EXC_H
#include "sistema.h"
#include "helpers.h"
#include "scheduler.h"
#include "interrupt.h"
#include "SYSCALL.h"

//HANDY DEFINES
#define INCREMENTO_PC current_p->p_s.pc_epc += WORDLEN
#define STATO_PROCESSO ((state_t *)BIOSDATAPAGE)

//FUNCTION PROTOTYPES
void exceptionHandler();
void PassUpOrDie(int excCode);

#endif

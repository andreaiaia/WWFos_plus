#ifndef HELP_H
#define HELP_H
#include "sistema.h"
#include "scheduler.h"
#include "SYSCALL.h"

// Helpers generici
void copy_state(state_t *original, state_t *dest);

// Helpers collegati alla Terminate_Process
pcb_PTR find_process(int pid);
void Exterminate(pcb_PTR);

// Helpers collegati al post syscall event (exception handler)

// Incremento PC e Caricamento BIOSDATAPAGE nel registro della CPU.
void post_syscall();
#endif
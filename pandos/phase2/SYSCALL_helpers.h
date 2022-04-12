#ifndef SYSH_H
#define SYSH_H
#include "sistema.h"
#include "scheduler.h"
#include "SYSCALL.h"

// Helpers generici
void copy_state(state_t *original, state_t *dest);

// Helpers collegati alla Terminate_Process
pcb_PTR find_process(int pid);
void Exterminate(pcb_PTR);

#endif
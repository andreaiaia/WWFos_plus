#ifndef SYSCALL_HELPERS_H
#define SYSCALL_HELPERS_H
#include "main.h"

// Helpers generici
void copy_state(state_t *original, state_t *dest);

// Helpers collegati alla Terminate_Process
void Exterminate(pcb_PTR process);
pcb_PTR find_process(int pid);

#endif
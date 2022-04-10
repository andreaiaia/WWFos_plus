#ifndef SYSH_H
#define SYSH_H
#include "sistema.h"
#include "scheduler.h"

// Helpers collegati alla Terminate_Process
void Exterminate(pcb_PTR process);
pcb_PTR find_process(int pid);

#endif
#ifndef HELP_H
#define HELP_H
#include "initial.h"
#include "scheduler.h"
#include "SYSCALL.h"

//* Helpers generici

// Copia lo stato di una cpu
void copy_state(state_t *original, state_t *dest);

//* Helpers collegati alla Terminate_Process

// Verifica se un processo è presente nella coda (sia bassa che alta priorità)
pcb_PTR find_process(int pid);
void Exterminate(pcb_PTR);

//* Helpers collegati all'exception handler

// SyscallExceptionHandler, gestisce il caso di un'eccezione di tipo Syscall e provvede allo smistamento
void syscallExceptionHandler(unsigned int syscallCode);
// Caricamento BIOSDATAPAGE nel registro della CPU e call allo scheduler()
void post_syscall();

void PassUpOrDie(int excCode);

#endif
#ifndef HELP_H
#define HELP_H
#include "initial.h"
#include "scheduler.h"
#include "SYSCALL.h"

//* Helpers generici */
/**
 * Funzione per copiare gli state (dato che la copia
 * di struct non è supportata nativamente)
 */
void copy_state(state_t *original, state_t *dest);

//* Helpers collegati alle Syscall */
void Exterminate(pcb_PTR process);
pcb_PTR find_process(int pid);

//* Helpers collegati all'exception handler */
// Operazioni post syscall
void postSyscall();
// SyscallExceptionHandler, gestisce il caso di un'eccezione di tipo Syscall e provvede allo smistamento
void syscallExceptionHandler(unsigned int syscallCode);
// Caricamento BIOSDATAPAGE nel registro della CPU e call allo scheduler()
void post_syscall();
// Funzione che termina il processo o passa l'eccezione al livello supporto
void PassUpOrDie(int excCode);

#endif
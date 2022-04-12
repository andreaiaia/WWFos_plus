#ifndef HELP_H
#define HELP_H
#include "sistema.h"
#include "scheduler.h"
#include "SYSCALL.h"

// ! HANDY DEFINES
#define DECODED_EXCEPTION_CAUSE CAUSE_GET_EXCCODE(((state_t *)BIOSDATAPAGE)->cause)
#define INCREMENTO_PC current_p->p_s.pc_epc += WORDLEN
#define STATO_PROCESSO ((state_t *)BIOSDATAPAGE) // * Shortcut per il Saved Status Process.
#define REG_A0_ST ((state_t *)BIOSDATAPAGE)->reg_a0 // * Shortcut per il reg_a0 che contiene il syscallCode.
#define REG_A1_ST ((state_t *)BIOSDATAPAGE)->reg_a1 // * Shortcut per il reg_a1 che contiene il 1° parametro da passare alla syscall.
#define REG_A2_ST ((state_t *)BIOSDATAPAGE)->reg_a2 // * Shortcut per il reg_a2 che contiene il 2° parametro da passare alla syscall.
#define REG_A3_ST ((state_t *)BIOSDATAPAGE)->reg_a3 // * Shortcut per il reg_a3 che contiene il 3° parametro da passare alla syscall.

// ! Helpers generici

void copy_state(state_t *original, state_t *dest);

// ! Helpers collegati alla Terminate_Process
pcb_PTR find_process(int pid);
void Exterminate(pcb_PTR);

// ! Helpers collegati all'exception handler

// * Caricamento BIOSDATAPAGE nel registro della CPU e call allo scheduler()
void post_syscall();

// * SyscallExceptionHandler, gestisce il caso di un'eccezione di tipo Syscall e provvede allo smistamento
void syscallExceptionHandler(unsigned int syscallCode);

// * Utility per il "passupordie"
void PassUpOrDie(int excCode);


#endif
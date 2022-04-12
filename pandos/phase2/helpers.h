#ifndef HELP_H
#define HELP_H
#include "sistema.h"
#include "scheduler.h"
#include "SYSCALL.h"

// ! HANDY DEFINES
#define INCREMENTO_PC current_p->p_s.pc_epc += WORDLEN
#define STATO_PROCESSO ((state_t *)BIOSDATAPAGE) // * Macro per il SavedStatus.
#define DECODED_EXCEPTION_CAUSE CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) //* Macro per l'exception code
#define REG_A0_SS STATO_PROCESSO->reg_a0 // * Macro per reg_a0 in SavedStatus(BIOSDATAPAGE) che contiene il syscallCode.
#define REG_A1_SS STATO_PROCESSO->reg_a1 // * Macro per reg_a1 in SavedStatus(BIOSDATAPAGE) che contiene il 1° parametro da passare alla syscall.
#define REG_A2_SS STATO_PROCESSO->reg_a2 // * Macro per reg_a2 in SavedStatus(BIOSDATAPAGE) che contiene il 2° parametro da passare alla syscall.
#define REG_A3_SS STATO_PROCESSO->reg_a3 // * Macro per reg_a3 in SavedStatus(BIOSDATAPAGE) che contiene il 3° parametro da passare alla syscall.

// ! Helpers generici

// * Copia lo stato di una cpu
void copy_state(state_t *original, state_t *dest);

// ! Helpers collegati alla Terminate_Process

// * Verifica se un processo è presente nella coda (sia bassa che altra priorità)
pcb_PTR find_process(int pid);

//* "Do you know how you make someone into a Dalek? Subtract Love, add Anger." ~ Steven Moffat
void Exterminate(pcb_PTR);

// ! Helpers collegati all'exception handler

// * Caricamento BIOSDATAPAGE nel registro della CPU e call allo scheduler()
void post_syscall();

// * SyscallExceptionHandler, gestisce il caso di un'eccezione di tipo Syscall e provvede allo smistamento
void syscallExceptionHandler(unsigned int syscallCode);

// * Utility per il "passupordie"
void PassUpOrDie(int excCode);


#endif
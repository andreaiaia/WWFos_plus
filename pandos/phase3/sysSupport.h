#ifndef SYS_SUPPORT_H
#define SYS_SUPPORT_H


#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "helpersSupport.h"
#include "initProc.h"
//* Costanti

#define GETTOD 1
#define TERMINATE 2
#define WRITEPRINTER 3
#define WRITETERMINAL 4
#define READTERMINAL 5

#define SWAP ((32 * PAGESIZE) + RAMSTART)
#define DEV_POSITION(T) ((int)T - DEV_REG_START) / (DEV_REG_SIZE / 2)

#define  SUP_REG_A0 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_a0
#define  SUP_REG_A1 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_a1
#define  SUP_REG_A2 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_a2

//* Richiami esterni
extern int device_sem[DEVSEM_NUM];
extern int getDeviceSemaphoreIndex(int line, int device, int term_is_recv);

//* Dichiarazioni prototipi di funzioni

/* General Exception Handler, gestisce le system call con numero >= 1 */
void generalExcHandler();

/* SYSCALL Exception Handler */
void syscallExcHandler(support_t *);

/* Wrapper per NSYS2: uccide lo user-process */
void trapExcHandler(support_t *);

//* INIZIO SYSCALL LVL SUPPORTO 

/* When this service is requested, it causes the number of microseconds since
the system was last booted/reset to be placed/returned in the U-proc’s v0
register. */
unsigned int getTod(support_t *currSupStructPTR);

/* This services causes the executing U-proc to cease to exist. The SYS2 service
is essentially a user-mode “wrapper” for the kernel-mode restricted NSYS2
service.*/
void terminate(support_t *currSupStructPTR);

/* When requested, this service causes the requesting U-proc to be suspended
until a line of output (string of characters) has been transmitted to the printer
device associated with the U-proc. */
int writeToPrinter(support_t *currSupstructPTR, char *virtAddrPTR, int len);

/* When requested, this service causes the requesting U-proc to be suspended
until a line of output (string of characters) has been transmitted to the
terminal device associated with the U-proc. */
int writeToTerminal(support_t *currSupStructPTR, char *virtAddrPTR, int len);

/* When requested, this service
causes the requesting U-proc to be suspended until a line of input (string
of characters) has been transmitted from the terminal device associated with
the U-proc. */
int readFromTerminal(support_t *currSupStructPTR, char *virtAddrPTR);

#endif
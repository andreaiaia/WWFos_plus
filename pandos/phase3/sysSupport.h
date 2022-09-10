#ifndef SYS_SUPPORT_H
#define SYS_SUPPORT_H

#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "helpersSupport.h"

//* Costanti

#define SWAP ((32 * PAGESIZE) + RAMSTART)
#define DEV_POSITION(T) ((int)T - DEV_REG_START) / (DEV_REG_SIZE / 2)

//* Richiami esterni
extern int device_sem[DEVSEM_NUM];
extern int getDeviceSemaphoreIndex(int line, int device, int term_is_recv);

//* Dichiarazioni di funzioni

/* General Exception Handler, gestisce le system call con numero >= 1 */
void generalExcHandler();

/* SYSCALL Exception Handler */
void syscallExcHandler(support_t *);

/* Wrapper per NSYS2: uccide lo user-process */
void trapExcHandler(support_t *);
#endif
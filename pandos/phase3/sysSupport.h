#ifndef SYS_SUPPORT_H
#define SYS_SUPPORT_H

#include "pandos_types.h"
#include "helpersSupport.h"

//* Costanti

#define DEVSEM_NUM 49
#define DEV_POSITION(T) ((int)T - DEV_REG_START) / (DEV_REG_SIZE / 2)

//* Richiami esterni
extern int device_sem[DEVSEM_NUM];
extern int getDeviceSemaphoreIndex(int line, int device, int term_is_recv);

//* Dichiarazioni di funzioni

/* General Exception Handler, gestisce le system call con numero >= 1 */
void generalExcHandler();

/* Wrapper per NSYS2: uccide lo user-process */
void trapExceptionHandler();
#endif
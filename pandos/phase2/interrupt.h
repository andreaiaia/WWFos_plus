#ifndef INT_H
#define INT_H
#include "sistema.h"
#include "scheduler.h"
void interruptHandler();
void PLTTimerInterrupt(int line);      // 3.6.2 pandos
void intervalTimerInterrupt(int line); // 3.6.3 pandos
void nonTimerInterrupt(int line);      // 3.6.1 pandos
extern int device_sem[DEVSEM_NUM];
void copyProcessorState(state_t *destination, state_t *source);
#endif
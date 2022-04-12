#ifndef INT_H
#define INT_H
#include "sistema.h"
#include "scheduler.h"
#include "helpers.h"

// Importo le variabili globali necessarie
extern int device_sem[DEVSEM_NUM];

void interruptHandler();
void PLTTimerInterrupt(int line);      // 3.6.2 pandos
void intervalTimerInterrupt(int line); // 3.6.3 pandos
void nonTimerInterrupt(int line);      // 3.6.1 pandos

#endif
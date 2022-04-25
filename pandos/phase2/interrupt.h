#ifndef INT_H
#define INT_H
#include "initial.h"
#include "scheduler.h"
#include "helpers.h"

//* Richiamo le variabili globali usate */
extern int device_sem[DEVSEM_NUM];

void interruptHandler();
void PLTTimerInterrupt(int line);
void intervalTimerInterrupt(int line);
void deviceInterrupt(int line);

#endif
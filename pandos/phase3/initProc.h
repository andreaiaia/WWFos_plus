#ifndef INIT_PROC_H
#define INIT_PROC_H

#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include <umps3/umps/cp0.h>
#include "vmSupport.h"

int printer_sem[UPROCMAX];
int flash_sem[UPROCMAX];
int term_w_sem[UPROCMAX];
int term_r_sem[UPROCMAX];

int mainSemaphore;

/** Strutture di supporto per i processi utente */
support_t support_table[UPROCMAX];

extern void initSwapStructs();

void test_fase3();

#endif
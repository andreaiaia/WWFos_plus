#ifndef INIT_PROC_H
#define INIT_PROC_H

#include <umps3/umps/arch.h>
#include <umps3/umps/const.h>
#include <umps3/umps/cp0.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/regdef.h>
#include <umps3/umps/types.h>
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "vmSupport.h"
#include "sysSupport.h"

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
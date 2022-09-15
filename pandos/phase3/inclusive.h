#ifndef INCLUSIVE_H
#define INCLUSIVE_H

#include <umps3/umps/arch.h>
#include <umps3/umps/const.h>
#include <umps3/umps/cp0.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/regdef.h>
#include <umps3/umps/types.h>
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"

int printer_sem[UPROCMAX];
int flash_sem[UPROCMAX];
int term_w_sem[UPROCMAX];
int term_r_sem[UPROCMAX];

int mainSemaphore;

// Swap pool table e relative strutture dati
swap_t swap_pool_table[POOLSIZE];
int swapSemaphore;

/** Strutture di supporto per i processi utente */
support_t support_table[UPROCMAX];



#endif
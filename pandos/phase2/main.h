#ifndef MAIN_H
#define MAIN_H
#include <umps3/umps/aout.h>
#include <umps3/umps/arch.h>
#include <umps3/umps/bios_defs.h>
#include <umps3/umps/const.h>
#include <umps3/umps/cp0.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/regdef.h>
#include <umps3/umps/types.h>
#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"
#include "scheduler.h" 
#include "exception.h"
#include "interrupt.h"
#include "SYSCALL_helpers.h"
#include "SYSCALL.h"

// Passup Vector
passupvector_t *pu_vector;

//* Dichiarazioni di funzioni esterne */
// Funzione fornita dalle specifiche, la riscriveremo nella prossima fase
extern void uTLB_RefillHandler();
// La funzione test si trova nel file di test fornito
extern void test();

#endif
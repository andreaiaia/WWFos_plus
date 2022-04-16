#ifndef SYS_H
#define SYS_H

#include <umps3/umps/arch.h>
#include <umps3/umps/const.h>
#include <umps3/umps/cp0.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/regdef.h>
#include <umps3/umps/types.h>
#include "../h/pandos_types.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"

//* Dichiarazioni di funzioni esterne */
// Funzione fornita dalle specifiche, la riscriveremo nella prossima fase
extern void uTLB_RefillHandler();
// La funzione test si trova nel file di test fornito
extern void test();
// Funzione utile per il debugging
extern void klog_print(char *);
extern void klog_print_hex(unsigned int);

#endif
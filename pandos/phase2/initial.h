#ifndef INIT_H
#define INIT_H

#include <umps3/umps/arch.h>
#include <umps3/umps/const.h>
#include <umps3/umps/cp0.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/regdef.h>
#include <umps3/umps/types.h>
#include "../h/pandos_types.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"

//* Handy defines */
// = 0xFFFF.FFFF per ricaricare PLTTimer
#define MAX_TIME 4294967295
// Numero di semafori dei dispositivi
#define DEVSEM_NUM 49
// Macro per il Saved Status
#define PROCESSOR_SAVED_STATE ((state_t *)BIOSDATAPAGE)
#define INCREMENTO_PC PROCESSOR_SAVED_STATE->pc_epc += WORDLEN
// Macro per l'exception code
#define DECODED_EXCEPTION_CAUSE CAUSE_GET_EXCCODE(PROCESSOR_SAVED_STATE->cause)
// Macro per reg_a0 in SavedStatus(BIOSDATAPAGE) che contiene il syscallCode.
#define REG_A0_SS PROCESSOR_SAVED_STATE->reg_a0
// Macro per reg_a1 in SavedStatus(BIOSDATAPAGE) che contiene il 1° parametro da passare alla syscall.
#define REG_A1_SS PROCESSOR_SAVED_STATE->reg_a1
// Macro per reg_a2 in SavedStatus(BIOSDATAPAGE) che contiene il 2° parametro da passare alla syscall.
#define REG_A2_SS PROCESSOR_SAVED_STATE->reg_a2
// Macro per reg_a3 in SavedStatus(BIOSDATAPAGE) che contiene il 3° parametro da passare alla syscall.
#define REG_A3_SS PROCESSOR_SAVED_STATE->reg_a3

//* Dichiarazioni di funzioni esterne */
// La funzione test si trova nel file di test fornito
extern void test_fase3();
// Funzioni utile per il debugging
extern void klog_print(char *);
extern void klog_print_hex(unsigned int);

#endif
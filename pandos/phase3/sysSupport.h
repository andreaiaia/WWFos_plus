#ifndef SYS_SUPPORT_H
#define SYS_SUPPORT_H

#include <umps3/umps/arch.h>
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "helpersSupport.h"
#include "initProc.h"

// Costanti
#define GETTOD 1
#define TERMINATE 2
#define WRITEPRINTER 3
#define WRITETERMINAL 4
#define READTERMINAL 5

// Macro
#define SWAP ((32 * PAGESIZE) + RAMSTART)
#define DEV_POSITION(T) ((int)T - DEV_REG_START) / (DEV_REG_SIZE / 2)

#define SUP_REG_A0 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_a0
#define SUP_REG_A1 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_a1
#define SUP_REG_A2 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_a2
#define SUP_REG_V0 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_v0

#define INC_PC currSupStructPTR->sup_exceptState[GENERALEXCEPT].pc_epc += 4

// Richiami esterni
extern int printer_sem[UPROCMAX];
extern int flash_sem[UPROCMAX];
extern int term_w_sem[UPROCMAX];
extern int term_r_sem[UPROCMAX];
extern int swapSemaphore;
extern int mainSemaphore;
extern int getDeviceSemaphoreIndex(int line, int device, int term_is_recv);
// support_t supportStruct_table[UPROCMAX];
extern struct list_head supportStruct_free;

//* Dichiarazioni prototipi di funzioni
// General Exception Handler, gestisce le system call con numero >= 1 */
void generalExcHandler();
// SYSCALL Exception Handler */
void syscallExcHandler(support_t *);
// Uccide il processo corrente in maniera ordinata */
void trapExcHandler(support_t *);

//* INIZIO SYSCALL LVL SUPPORTO
/**
 * Il servizio restituisce in v0 il numero di microsecondi
 * passati da quando il sistema è stato avviato.
 */
void getTod(support_t *currSupStructPTR);

/**
 * Questo servizio ammazza il current process.
 * È essenzialmente un wrapper della TERMPROCESS di fase 2.
 */
void terminate(support_t *currSupStructPTR);

/**
 * Questo servizio sospende il processo chiamante fino a
 * quando una linea di output non è stata trasmessa al printer
 * device proprio del processo.
 */
void writeToPrinter(support_t *currSupStructPTR, char *virtAddrPTR, int len);

/**
 * Questo servizio sospende il processo chiamante fino a
 * quando una linea di output non è stata trasmessa al terminal
 * device proprio del processo.
 */
void writeToTerminal(support_t *currSupStructPTR, char *virtAddrPTR, int len);

/**
 * Questo servizio sospende il processo chiamante fino a
 * quando una linea di input non è stata trasmessa dal terminal
 * device proprio del processo.
 */
void readFromTerminal(support_t *currSupStructPTR, char *virtAddrPTR);

#endif
#ifndef SYS_SUPPORT_H
#define SYS_SUPPORT_H

#include "inclusive.h"

// Costanti
#define GETTOD 1
#define TERMINATE 2
#define WRITEPRINTER 3
#define WRITETERMINAL 4
#define READTERMINAL 5

// Macro
#define DEV_POSITION(T) ((int)T - DEV_REG_START) / (DEV_REG_SIZE / 2)

#define SUP_REG_A0 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_a0
#define SUP_REG_A1 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_a1
#define SUP_REG_A2 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_a2
#define SUP_REG_V0 currSupStructPTR->sup_exceptState[GENERALEXCEPT].reg_v0

#define INC_PC currSupStructPTR->sup_exceptState[GENERALEXCEPT].pc_epc += 4


//* Dichiarazioni prototipi di funzioni
// General Exception Handler, gestisce le system call con numero >= 1 */
void generalExcHandler();
// SYSCALL Exception Handler */
void syscallExcHandler(support_t *);
// Uccide il processo corrente in maniera ordinata */
void trapExcHandler();

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
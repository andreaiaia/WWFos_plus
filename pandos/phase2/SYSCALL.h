#ifndef SYSC_H
#define SYSC_H
#include "sistema.h"
#include "scheduler.h"

/**
 * Per trovare il numero di device io passo l'indirizzo del commandAddr
 * Tolgo l'offset (DEV_REG_START) e così posso dividere per la dimensione
 * del blocco di memoria che contiene un dispositivo. Essendo una divisione
 * intera il numero risultante corrisponde all'indice del relativo semaforo
 * nell'array dei semafori. Una volta che so la posizione del semaforo è checkmate.
 */
#define DEV_POSITION(T) ((int)T - DEV_REG_START) / (DEV_REG_SIZE / 2)

// Questa system call crea un nuovo processo come figlio del chiamante.
void Create_Process(state_t *statep, int prio, support_t *supportp);

/** Quando invocata, la SYS2 termina il processo indicato
 * dal secondo parametro insieme a tutta la sua progenie.
 */
void Terminate_Process(int pid);

// SYSCALL sui semafori
void Passeren(int *semaddr);
void Verhogen(int *semaddr);

void Do_IO_Device(int *commandAddr, int commandValue);

void Get_CPU_Time();

void Wait_For_Clock();

void Get_Support_Data();

#endif
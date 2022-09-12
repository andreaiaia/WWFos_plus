#ifndef SYSC_H
#define SYSC_H
#include "initial.h"
#include "scheduler.h"
#include "helpers.h"

/**
 * Per trovare il numero di device io passo l'indirizzo del commandAddr
 * Tolgo l'offset (DEV_REG_START) e così posso dividere per la dimensione
 * del blocco di memoria che contiene un dispositivo. Essendo una divisione
 * intera il numero risultante corrisponde all'indice del relativo semaforo
 * nell'array dei semafori. Una volta che so la posizione del semaforo è checkmate.
 */
#define DEV_POSITION(T) ((int)T - DEV_REG_START) / (DEV_REG_SIZE / 2)
/* Richiamo le variabili globali necessarie */
extern int proc_count, soft_count, device_sem[DEVSEM_NUM];
extern pcb_PTR current_p, yielded;
extern pcb_PTR all_processes[MAXPROC];

/**
 * Questa syscall crea un nuovo processo come progenie,
 * del chiamante.
 * @param a1 contiene il puntatore allo stato state_t da
 * dare come stato iniziale al processo creato.
 * @param prio specifica in quale ready_queue vada inserito
 * il processo creato.
 * @param supportp puntatore alla struttura di supporto da dare
 * al processo creato.
 */
void Create_Process(state_t *statep, int prio, support_t *supportp);

/**
 * Questa syscall termina il processo indicato dal pid
 * passato e termina anche tutti i suoi processi figli.
 * @param pid indica il process id del processo da terminare,
 * se il parametro è 0 allora il processo da terminare è il chiamante.
 */
int Terminate_Process(int pid);

//* SYSCALL sui semafori */

/**
 * Questa syscall effettua una P sul semaforo indicato.
 * @param semaddr puntatore al semaforo su cui fare la P.
 */
int Passeren(int *semaddr);

/**
 * Questa syscall effettua una V sul semaforo indicato.
 * @param semaddr puntatore al semaforo su cui fare la V.
 */
pcb_PTR Verhogen(int *semaddr);

/**
 * Questa syscall gestisce i servizi di I/O.
 * @param commandAddr è il puntatore al campo command del
 * dispositivo da utilizzare per l'operazione I/O.
 * @param commandValue è il valora da leggere/scrivere usando
 * il dispositivo.
 */
int Do_IO_Device(int *commandAddr, int commandValue);

/**
 * Questa syscall restituisce il tempo di processore usato
 * complessivamente dal processo chiamante.
 */
void Get_CPU_Time();

/**
 * Questa syscall esegue una P sullo pseudo-clock semaphore
 * del kernel. Questa operazione è sempre bloccante.
 */
void Get_Process_Id(int parent);

/**
 * Questa syscall restituisce un puntatore alla struttura di
 * supporto del processo corrente.
 */
int Wait_For_Clock();

/**
 * Questa syscall restituisce il pid del processo o del suo genitore.
 * @param parent se vale 0 la syscall restituisce il pid del processo
 * chiamante, altrimenti del suo genitore
 */
void Get_Support_Data();

/**
 * Questa syscall sospende il processo corrente,
 * rimettendono alla fine della sua ready_queue.
 */
void Yield();

#endif
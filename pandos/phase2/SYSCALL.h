#include <umps3/umps/libumps.h>
#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"
#include "main.h"
#include "scheduler.h"
#include "SYSCALL_helpers.h"

/**
 * Per trovare il numero di device io passo l'indirizzo del commandAddr
 * Tolgo l'offset (DEV_REG_START) e così posso dividere per la dimensione
 * del blocco di memoria che contiene un dispositivo. Essendo una divisione
 * intera il numero risultante corrisponde all'indice del relativo semaforo
 * nell'array dei semafori. Una volta che so la posizione del semaforo è checkmate.
 */
#define DEV_POSITION(T) ((int)T - DEV_REG_START) / DEV_REG_SIZE

// Richiamo alle variabili esterne
extern int proc_count;
extern struct list_head *high_ready_q, *low_ready_q;
extern pcb_PTR current_p;

// Questa system call crea un nuovo processo come figlio del chiamante.
int Create_Process(state_t *statep, int prio, support_t *supportp);

/** Quando invocata, la SYS2 termina il processo indicato
 * dal secondo parametro insieme a tutta la sua progenie.
 */
void Terminate_Process(int pid);

// SYSCALL sui semafori
void Passeren(int *semaddr);
void Verhogen(int *semaddr);

// DOIO
void Do_IO_Device(int *commandAddr, int commandValue);
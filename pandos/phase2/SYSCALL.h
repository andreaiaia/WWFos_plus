#include <umps3/umps/libumps.h>
#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"
#include "main.h"
#include "scheduler.h"
#include "SYSCALL_helpers.h"

// Richiamo alle variabili esterne
extern int proc_count;
extern struct list_head *high_ready_q, *low_ready_q;
extern pcb_PTR current_p;

// Questa system call crea un nuovo processo come figlio del chiamante.
int Create_Process(state_t *statep, int prio, support_t *supportp);

/** Quando invocata, la SYS2 termina il processo indicato
 * dal secondo parametro insieme a tutta la sua progenie.
 */
void Terminate_Process(int pid, 0, 0);

// SYSCALL sui semafori
void Passeren(int *semaddr, 0, 0);
void Verhogen(int *semaddr, 0, 0);

// DOIO
int Do_IO_Device(int *cmdAddr, int cmdValue, 0);
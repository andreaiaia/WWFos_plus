#include <umps3/umps/libumps.h>
#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"
#include "main.h"
#include "scheduler.h"

// Questa system call crea un nuovo processo come figlio del chiamante.
int Create_Process(state_t *statep, int prio, support_t *supportp);

/** Quando invocata, la SYS2 termina il processo indicato
 * dal secondo parametro insieme a tutta la sua progenie.
 */
void Terminate_Process(int pid, 0, 0);
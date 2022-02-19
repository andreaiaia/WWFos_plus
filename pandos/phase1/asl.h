#include "../h/pandos_const.h"
#include "../h/pandos_types.h"
#include "../h/listx.h"

/* Gestione della ASL (Active Semaphore List) */

int insertBlocked(int *semAdd, pcb_t *p);

pcb_t *removeBlocked(int *semAdd);

pcb_t *outBlocked(pcb_t *p);

pcb_t *headBlocked(int *semAdd);

void initASL();
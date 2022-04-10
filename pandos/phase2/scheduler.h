#ifndef SCH_H
#define SCH_H
#include "sistema.h"
//* Costanti */
// Numero di semafori dei dispositivi
#define DEVSEM_NUM 49

void load_new_proc(struct list_head *);

void scheduler();

#endif
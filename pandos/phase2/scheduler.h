#ifndef SCH_H
#define SCH_H
#include "initial.h"

//* Costanti */
extern struct list_head high_ready_q;
extern struct list_head low_ready_q;
extern pcb_PTR current_p, yielded;
extern int proc_count;
extern int soft_count;
extern cpu_t start;
extern cpu_t finish;

void load_new_proc(struct list_head *);
void scheduler();

#endif
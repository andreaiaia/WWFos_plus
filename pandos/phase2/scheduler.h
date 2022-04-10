#ifndef SCH_H
#define SCH_H
#include "sistema.h"
//* Costanti */
// Numero di semafori dei dispositivi
#define DEVSEM_NUM 49

//* Variabili Globali */
// Process Count - Contatore processi vivi (started but not yet finished)
int proc_count;
// Soft-Block Count - Contatore dei processi avviati ma non ancora terminati (e quindi bloccati)
int soft_count;
// Queue dei processi ad alta priorità
struct list_head *high_ready_q;
// Queue dei processi a bassa priorità
struct list_head *low_ready_q;
// Current Process - Puntatore a pcb in stato "Running" (correntemente attivo)
pcb_PTR current_p;
// Device Semaphores - we need 49 sem in total
// Ultimo semaforo è il pseudo-clock semaphore
int device_sem[DEVSEM_NUM];

cpu_t start;
cpu_t finish;

void load_new_proc(struct list_head *);

void scheduler();

#endif
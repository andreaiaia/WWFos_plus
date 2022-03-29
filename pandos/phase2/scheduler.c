#include "scheduler.h"

// Queue dei processi ad alta priorità
list_head *high_ready_q;
// Queue dei processi a bassa priorità
list_head *low_ready_q;

// Richiamo le variabili globali dal main
extern int proc_count;
extern int soft_count;
extern pcb_PTR current_p;
extern int device_sem[DEVSEM_NUM];
extern passupvector_t *pu_vector;

void scheduler()
{
    // Se la coda dei processi ad ALTA priorità è non-vuota
    if (!emptyProcQ(high_ready_q))
    {
        current_p = removeProcQ(high_ready_q);
        LDST(&(current_p->p_s));
    }
    // Se la coda dei processi a BASSA priorità è non-vuota
    else if (!emptyProcQ(low_ready_q))
    {
        current_p = removeProcQ(low_ready_q);
        setTimer(50000);
        LDST(&(current_p->p_s));
    }
    // Se le code sono entrambe vuote
    else
    {
        if (proc_count == 0)
            HALT(); // Spegne il computer
        else if (soft_count > 0)
        {
            // Imposto lo stato corrente per accettare interrupt
            // E disabilito il tutto il resto (quindi anche il PLT)
            // ? non sono sicuro vada usato lo state del processo corrente
            current_p->p_s.status = IECON | IMON;

            WAIT(); // Aspettando un interrupt
        }
        else
        {
            // Siamo in deadlock
            PANIC();
        }
    }
}
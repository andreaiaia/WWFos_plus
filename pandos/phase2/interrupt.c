#include "../umps/cp0.h"
#include "../umps/libumps.h"
#include "../h/pandos_const.h"
#include "main.c"
#include "scheduler.h"

/*
  ! NOTE
  - occhio alle linee 3-7, per quelle c'é Mappa che indica i pending interrupt
  - nella stessa linea posso avere 2 tipi di richiesta:
    timer (richiesta interna) o periferica (richiesta esterna)
  - inoltre ci sono 2 tipi di trasmissione: read e write
    write ha priorità piu' alta rispetto alla ricezione (lettura)

  ! MACRO
  #define CAUSE_IP_MASK          0x0000ff00
  #define CAUSE_IP(line)         (1U << (8 + (line)))
  #define CAUSE_IP_BIT(line)     (8 + (line))

  Interrupting devices bitmap (file arch.h)
  #define CDEV_BITMAP_BASE        0x10000040
  #define CDEV_BITMAP_END         (CDEV_BITMAP_BASE + N_EXT_IL * WS)
  #define CDEV_BITMAP_ADDR(line)  (CDEV_BITMAP_BASE + ((line) - DEV_IL_START) * WS)

  ! DEPRECATE
  int cause_reg = getCAUSE(); // accedo a registro Cause
  cause_reg = cause_reg && CAUSE_IP_MASK;  // estraggo campo IP tramite AND bit a bit

  ! USEFUL INFO
  (4.1.3 pops)
  Interval Timer è un 32bit value che decrementa di 1 a ogni ciclo del processore e genera
  un interrupt nella linea 2 ogni volta che fa una transizione 0x0000.0000 -> 0xFFFF.FFFF
  Tutti gli interrupt nella linea 2 sono associati a Interval Timer

  (4.1.4)
  A PLT is the only device attached to the interrupt line 1
*/

extern struct list_head *high_ready_q;
extern struct list_head *low_ready_q;
extern device_sem[DEVSEM_NUM];

void interruptHandler()
{
  int line = 1; // interrupt line, linea 0 da ignorare
  for (line = 1; line < 8; line++)
  {
    if (CAUSE_IP_BIT(line) == 1) // bit di i-esima linea = 1 c'è interrupt pending
    {
      if (line == 1)
        PLTTimerInterrupt(line);
      else if (line == 2)
        intervalTimerInterrupt(line);
      else
        nonTimerInterrupt(line);
    }
  }
}

// * linea 1   (3.6.2 pandos)
void PLTTimerInterrupt(int line)
{
  // acknowledgement del PLT interrupt (4.1.4-pops)
  setTIMER(100); // ! che valore caricare?  100 è placeholder, sul git caricano __INT32_MAX__
  // ottengo e copio stato processore (che si trova all'indirizzo 0x0FFF.F000, 3.2.2-pops) nel pcb attuale
  state_t processor_state = *((state_t *)0x0FFFF000) // ! questo casting l'ho scopiazzato, mi è chiaro cosa fa
                             current_p->p_s = processor_state;
  // metto current process in Ready Queue e da "running" lo metto in "ready"
  insertProcQ(low_ready_q, current_p); // ! messa in low queue, forse va in high, o forse da fare if else per distinguere

  current_p->p_s->status = 1; // ! valore 1 è placeholder, da capire come mettere in "ready"
  // ? actually la riga qui sopra non credo serva, perché il "transitioning" da running a ready
  // ? viene fatto appunto mettendo il processo nella coda dei ready
  scheduler();
}

// * linea 2   (3.6.3 pandos)
void intervalTimerInterrupt(int line)
{
  // acknowledgement dell'interrupt (4.1.3-pops)
  LDIT(PSECOND); // carico Interval Timer con 100millisec
  // sblocco tutti i pcb bloccati nel Pseudo-clock semaphore
  // TODO
  // resetto il Pseudo-clock semaphore a 0
  device_sem[48] = 0;
  // ritorno controllo a processo corrente e faccio LDST nell'exception state salvato
  // ritornare controllo  -> fare load_state e caricare stato nel processore
  LDST(0x0FFFF000);
  // ? bisogna incrementare PC qui?
}

// * linee 3-7    (3.6.1 pandos)
void nonTimerInterrupt(int line)
{
  int device_num = 0;
  // calcolo il n° del device che ha generato l'interrupt nella line
  for (int dev = 0; dev < DEVPERINT; dev++) // scorro gli 8 device della linea
  {
    // TODO: scorrere IDBM (5.2.2-pops) alla ricerca di bit a 1 per vedere device con interrupt pending
    // TODO: scorro la line+3 esima word e l'i-esimo bit sarà il DevNo che mi serviva
    // TODO: quindi accedo al bit i-esimo della line-3 esima word e ho l'indirizzo del device
  }

  // 1. calcolare indirizzo del device's device register
  int dev_addr_base = 0x1000.0054 + ((line - 3) * 0x80) + (device_num * 0x10); // pag. 28 manuale pops
  // 2. salvare lo status code

  // 3. acknowledgement dell'interrupt

  // 4. Verhogen sul semaforo associato al device (sblocco pcb e metto in ready)

  // 5. metto lo status code salvato precedentemente nel registro v0 del pcb appena sbloccato

  // 6. inserisco il pcb sbloccato nella ready queue, processo passa da "blocked" a "ready"

  // 7. ritorno controllo al processo corrente
  LDST(0x0FFFF000);
  // ? bisogna incrementare PC qui?
}

// TODO:
/*
- tenere traccia del momento di inizio dell'interrupt
- trovare interrupt con priorità piu' alta (in teoria va bene quello che ho fatto)
  - ottengo numero linea con cui farò uno switch
    - linea 0 non fa nulla
    - linea 1 -> PLT timer interrupt line         (3.6.2)
    - linea 2 -> Interval Timer Interrupt Line    (3.6.1)
    - linee 3-7 -> Non-Timer interrupt lines      (3.6.3)
      - una volta individuata la linea, bisogna ciclare sui device di quella linea (DEVPERINT)
        - una volta individuato il device

*/
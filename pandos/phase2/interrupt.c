#include "interrupt.h"

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

  ! COMANDI GIT
  git add .
  git commit -m "nome_commit"
  git push

  ? DOMANDE
  ? quali sono i device di tipo "terminal"? inoltre nei terminal bisogna distinguere i 2 casi sub-device
    ? la trasmissione ha priorità su ricezione
  * la linea terminal dovrebbe essere la 7 da quanto scritto su 3.6
***************************************************************************************************************************************/

#define UNSIGNED_MAX_32_INT 4294967295

void interruptHandler()
{
  // TODO controllare tramite la mask se l'interrupt è lecito
  for (int line = 1; line < 8; line++) // linea 0 da ignorare
  {
    if (((state_t *)BIOSDATAPAGE)->cause & CAUSE_IP(line))
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

//* linea 1   (3.6.2 pandos)
void PLTTimerInterrupt(int line)
{
  // acknowledgement del PLT interrupt (4.1.4-pops)
  setTIMER(UNSIGNED_MAX_32_INT); // ricarico valore 0xFFFF.FFFF
  // ottengo e copio stato processore (che si trova all'indirizzo 0x0FFF.F000, 3.2.2-pops) nel pcb attuale
  state_t *processor_state = getSTATUS(); // ! secondo me (nick) qui va messo processor_state = ((state_t *)BIOSDATAPAGE)
  current_p->p_s = *processor_state;
  // metto current process in Ready Queue e da "running" lo metto in "ready"
  insertProcQ(low_ready_q, current_p);
  current_p = NULL; // perché lo scheduler altrimenti continua ad eseguirlo
  scheduler();
}

//* linea 2   (3.6.3 pandos)
void intervalTimerInterrupt(int line)
{
  // acknowledgement dell'interrupt (4.1.3-pops)
  LDIT(PSECOND); // carico Interval Timer con 100millisec
  // sblocco tutti i pcb bloccati nel Pseudo-clock semaphore
  while (removeBlocked(device_sem[48]))
    ;
  // resetto lo pseudo-clock semaphore a 0
  device_sem[48] = 0;
  if (current_p)
    LDST(BIOSDATAPAGE);
  else
    scheduler();
}

//* linee 3-7    (3.6.1 pandos)
void nonTimerInterrupt(int line)
{
  int device_num = 0;
  devregarea_t *device_regs = (devregarea_t *)RAMBASEADDR; // tutor: nel campo deviceRegs->interrupt_dev trovate la interrupt device bitmap
  unsigned int bitmap_word = device_regs->interrupt_dev[line - 3];

  //* 1. calcolare indirizzo del device's device register
  // calcolo il n° del device che ha generato l'interrupt nella line

  unsigned int mask = 1;
  int i = 0;
  int flag = 0;

  while ((i < 8) & (flag == 0)) // scorro device
  {
    if (bitmap_word & mask) // device con interrupt pending trovato
    {
      device_num = i; // salvo n° device
      flag = 1;

      if (device_num == 7) // se è un terminale
      {
        termreg_t *device_ptr = DEV_REG_ADDR(line, device_num);

        if (device_ptr->transm_status == 1) // terminale ha priorità di trasmissione piu' alta rispetto a ricezione
        {
          // ? al momento non so come usare questa info, per ora ho capito come distinguere la priorità
        }
      }
    }
    mask = mask * 2;
  }

  // ottengo il device's device register
  dtpreg_t *device_ptr = DEV_REG_ADDR(line, device_num); // pag. 28 manuale pops
  //* 2. salvare lo status code
  unsigned int device_status = device_ptr->status;
  // 3. acknowledgement dell'interrupt
  device_ptr->command = 0; // TODO: trovare cosa scrivere come acknowledgement, 0 è placeholder
  // 4. Verhogen sul semaforo associato al device (sblocco pcb e metto in ready)

  // 5. metto lo status code salvato precedentemente nel registro v0 del pcb appena sbloccato

  // 6. inserisco il pcb sbloccato nella ready queue, processo passa da "blocked" a "ready"

  // 7. ritorno controllo al processo corrente
  LDST(BIOSDATAPAGE);
}

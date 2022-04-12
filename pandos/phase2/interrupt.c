#include "interrupt.h"

/*
  ! MACRO
  #define CAUSE_IP_MASK          0x0000ff00
  #define CAUSE_IP(line)         (1U << (8 + (line)))
  #define CAUSE_IP_BIT(line)     (8 + (line))

  Interrupting devices bitmap (file arch.h)
  #define CDEV_BITMAP_BASE        0x10000040
  #define CDEV_BITMAP_END         (CDEV_BITMAP_BASE + N_EXT_IL * WS)
  #define CDEV_BITMAP_ADDR(line)  (CDEV_BITMAP_BASE + ((line) - DEV_IL_START) * WS)

  ! COMANDI GIT
  git add .
  git commit -m "nome_commit"
  git push

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
  state_t *processor_state = ((state_t *)BIOSDATAPAGE);

  copy_state(&current_p->p_s, processor_state);

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
  while (removeBlocked(device_sem[DEVSEM_NUM - 1]))
    ;
  // resetto lo pseudo-clock semaphore a 0
  device_sem[DEVSEM_NUM - 1] = 0;
  if (current_p)
    LDST((STATE_PTR)BIOSDATAPAGE);
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

  unsigned int mask = 1, i = 0, flag = 0;
  int terminal_request = 0; // salva il tipo di richiesta del terminale, 0->trasmission, 1->receive

  while ((i < 8) & (flag == 0)) // scorro devices della line
  {
    if (bitmap_word & mask) // device con interrupt pending trovato
    {
      device_num = i; // salvo n° device
      flag = 1;

      if (line == 7) // se è un terminale
      {
        termreg_t *terminal_ptr = (termreg_t *)DEV_REG_ADDR(line, device_num); // calcolo indirizzo terminale

        if (terminal_ptr->transm_status == READY) // terminale ha priorità di trasmissione piu' alta rispetto a ricezione
          terminal_request = 1;
        else
          terminal_request = 0;
        // TODO Controllare la correttezza del calcolo del sem_num
      }
    }
    mask = mask * 2;
  }

  // ottengo il device's device register
  // ! correggere il tipo del puntatore a seconda di terminale o non terminale
  dtpreg_t *device_ptr = (dtpreg_t *)DEV_REG_ADDR(line, device_num);
  // 2. salvare lo status code
  unsigned int device_status_code = device_ptr->status;
  // 3. acknowledgement dell'interrupt
  device_ptr->command = ACK;
  // 4. Verhogen sul semaforo associato al device (sblocco pcb e metto in ready)
  int sem_num = 8 * (line - 3) + (line == 7 ? 2 * device_num : device_num) + terminal_request; // calcolo numero semaforo associato a device
  Verhogen(device_sem[sem_num]);
  Do_IO_Device();
  // 5. metto lo status code salvato precedentemente nel registro v0 del pcb appena sbloccato

  // 6. inserisco il pcb sbloccato nella ready queue, processo passa da "blocked" a "ready"

  // 7. ritorno controllo al processo corrente
  LDST((STATE_PTR)BIOSDATAPAGE);
}



/*
  ! COMANDI GIT
  git add .
  git commit -m "nome_commit"
  git push
*/

#include "interrupt.h"

#define UNSIGNED_MAX_32_INT 4294967295 // =0xFFFF.FFFF per ricaricare PLTTimer


void interruptHandler()
{
  // scorro linee di Cause.IP cercando interrupt pending
  for (int line = 1; line < 8; line++)  // linea 0 da ignorare
  {
    if (getCAUSE() & CAUSE_IP(line))
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


// linea 1   (3.6.2 pandos)
void PLTTimerInterrupt(int line)
{
  setTIMER(UNSIGNED_MAX_32_INT); // ricarico timer

  // copio stato processore nel pcb attuale
  state_t *processor_state = PROCESSOR_SAVED_STATE;
  copy_state(processor_state, &current_p->p_s);

  // metto current process in "ready"
  insertProcQ(low_ready_q, current_p); 
  current_p = NULL; // perché lo scheduler altrimenti continua ad eseguirlo

  scheduler();
}


// linea 2   (3.6.3 pandos)
void intervalTimerInterrupt(int line)
{
  LDIT(PSECOND); // carico Interval Timer con 100millisec

  // sblocco tutti i pcb bloccati nel Pseudo-clock semaphore
  while (removeBlocked(&(device_sem[DEVSEM_NUM - 1])));

  // resetto pseudo-clock semaphore
  device_sem[DEVSEM_NUM - 1] = 0;

  // se c'é un processo in esecuzione torno a quello altrimenti chiamo scheduler
  if (current_p)
    LDST((STATE_PTR)BIOSDATAPAGE);
  else
    scheduler();
}


// linee 3-7   (3.6.1 pandos)
void nonTimerInterrupt(int line)
{
  int device_num = 0;  
  devregarea_t *device_regs = (devregarea_t *)RAMBASEADDR; 
  // salvo word della line in cui trovo i device con interrupt pending
  unsigned int bitmap_word = device_regs->interrupt_dev[line - 3];  // tutor: nel campo deviceRegs->interrupt_dev trovate la interrupt device bitmap
  unsigned int device_status_code = 0;  
  unsigned int mask = 1, i = 0, flag = 0;
  int terminal_request = 0; // 0->receive, 1->transmission; per distinguere semaforo da usare

  // calcolo n° del device che ha generato l'interrupt nella line
  while ((i < 8) & (flag == 0)) // scorro devices della line
  {
    // device con interrupt pending trovato
    if (bitmap_word & mask) 
    {
      device_num = i; // salvo n° device
      flag = 1;
      // se è un terminale
      if (line == 7) 
      {
        termreg_t *terminal_ptr = (termreg_t *)DEV_REG_ADDR(line, device_num); // calcolo indirizzo terminale

        if (terminal_ptr->transm_status == READY){ // terminale ha priorità di trasmissione piu' alta rispetto a ricezione
          device_status_code = terminal_ptr->transm_status;  
          terminal_ptr->transm_command = ACK;
          terminal_request = 1;
        }
        else{
          device_status_code = terminal_ptr->recv_status; 
          terminal_ptr->recv_command = ACK;
          terminal_request = 0;
        }
      }
      else{ // non-terminale
        dtpreg_t *device_ptr = (dtpreg_t *)DEV_REG_ADDR(line, device_num);
        device_status_code = device_ptr->status;
        device_ptr->command = ACK;
      }
    }
    mask = mask * 2;
  }
  // calcolo semaforo associato a device
  int sem_num = 8 * (line - 3) + (line == 7 ? 2 * device_num : device_num) + terminal_request; 
  
  Verhogen(&(device_sem[sem_num]));
  //? pcb_PTR tmp = Verhogen();
  //? pcb_andrea->p_s.reg_v0 = device_status_code;

  LDST((STATE_PTR)BIOSDATAPAGE);
  scheduler();
}

#include "interrupt.h"

void interruptHandler()
{
  klog_print("INT_H - entro\n");
  /**
   * Scorro le linee di Cause.IP alla ricerca di un
   * pending interrupt, non guardo la linea 0 perché
   * il nostro sistema usa un solo processore
   */
  for (int line = 1; line < N_INTERRUPT_LINES; line++)
  {
    if ((PROCESSOR_SAVED_STATE->cause) & CAUSE_IP(line))
    {
      if (line == 1)
      {
        PLTTimerInterrupt(line);
        break;
      }
      else if (line == 2)
      {
        intervalTimerInterrupt(line);
        break;
      }
      else
      {
        klog_print_hex(line);
        klog_print("\n");
        nonTimerInterrupt(line);
        break;
      }
    }
  }
}

void PLTTimerInterrupt(int line)
{
  klog_print("PLT\n");
  setTIMER(MAX_TIME); // ricarico timer
  copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
  insertProcQ(&low_ready_q, current_p);
  current_p = NULL;
  scheduler();
}

// linea 2   (3.6.3 pandos)
void intervalTimerInterrupt(int line)
{
  klog_print("INT_TIMER_INT\n");
  LDIT(PSECOND); // carico Interval Timer con 100millisec
  klog_print("INT_TIMER_INT ho caricato timer\n");

  // sblocco tutti i pcb bloccati nel Pseudo-clock semaphore
  pcb_PTR removed = NULL;
  //klog_print("INT_TIMER_INT ho creato pcb\n");
  do
  { 
    removed = removeBlocked(&(device_sem[DEVSEM_NUM - 1]));
    //klog_print("INT_TIMER_INT ho rimosso\n");
    if (removed != NULL)
    {
      klog_print("Ho sbloccato un proc\n");
      if (removed->p_prio == 1)
        insertProcQ(&high_ready_q, removed);
      else
        insertProcQ(&low_ready_q, removed);
    }
  } while (removed != NULL);

  // Azzero lo pseudo-clock semaphore
  device_sem[DEVSEM_NUM - 1] = 0;
  if (current_p == NULL)
  {
    scheduler();
  }
  else
  {
    LDST(PROCESSOR_SAVED_STATE);
  }
}

// linee 3-7   (3.6.1 pandos)
void nonTimerInterrupt(int line)
{
  klog_print("NTI - nonTimerInterrupt\n");
  int device_num = 0;
  devregarea_t *dev_regs = (devregarea_t *)RAMBASEADDR;
  /**
   * Salvo la word della line in cui trovo i device con dei pending interrupt.
   * Nel campo deviceRegs->interrupt_dev c'è la interrupt device bitmap,
   * una matrice 5 x 8 con tutte le linee di interrupt dedicate ai dispositivi
   */
  unsigned int bitmap_word = dev_regs->interrupt_dev[line - 3];
  // Variabili utilizzate per lo scorrimento della matrice
  unsigned int dev_status_code = 0, mask = 1;
  /**
   * Uso un intero per distinguere quando il terminale sta ricevendo
   * da quando sta trasmettendo, in modo da trovare il giusto semaforo
   * * 1 = il terminale riceve
   * * 0 = il terminale trasmette
   */
  int term_is_recv = 0;

  // Scorro i dispositivi
  for (int i = 0; i < N_DEV_PER_IL; i++)
  {
    // Check per trovare il device con un pending interrupt
    if (bitmap_word & mask)
    {
      // Salvo il numero del device trovato
      device_num = i;
      // Se il dev è un terminal
      if (line == 7)
      {
        termreg_t *terminal_ptr = (termreg_t *)DEV_REG_ADDR(line, device_num); // calcolo indirizzo terminale

        /**
         * Controllo prima se c'è un interrupt sulla linea di trasmissione,
         * in quanto questa ha la priorità sulla ricezione
         */
        if (terminal_ptr->transm_status != READY)
        {
          dev_status_code = terminal_ptr->transm_status;
          terminal_ptr->transm_command = ACK;
          term_is_recv = 1;
          klog_print("ACK dato al term0 transm\n");
        }
        else if (terminal_ptr->recv_status != READY)
        {
          dev_status_code = terminal_ptr->recv_status;
          terminal_ptr->recv_command = ACK;
          term_is_recv = 0;
          klog_print("ACK dato al term0 rec\n");
        }
      }
      else
      {
        dtpreg_t *device_ptr = (dtpreg_t *)DEV_REG_ADDR(line, device_num);
        dev_status_code = device_ptr->status;
        device_ptr->command = ACK;
      }
      break;
    }
    mask *= 2;
  }
  // Ora che ho identificato il dispositivo corretto, risalgo al semaforo associato
  int sem_num = 8 * (line - 3) + (line == 7 ? 2 * device_num : device_num) + term_is_recv;
  klog_print("NTI sem_linea: ");
  klog_print_hex(sem_num);
  klog_print("\n");
  pcb_PTR tmp = Verhogen(&(device_sem[sem_num]));
  if (tmp != NULL){
    tmp->p_s.reg_v0 = dev_status_code; 
    soft_count--;
    }
  // copio stato processore nel pcb attuale
  klog_print("NTI copio stato processore\n");
  if (current_p == NULL)
  {
    klog_print("NTI current_p == NULL\n");
    scheduler();
  }
  else
  {
    klog_print("NTI kariko stato\n");
    LDST(PROCESSOR_SAVED_STATE);
  }
  // copy_state(PROCESSOR_SAVED_STATE, &(current_p->p_s));
  // insertProcQ(&low_ready_q, current_p);
  // scheduler();
  //  ? come mai non chiamiamo più lo scheduler? fatti i cazzi tuoi
}

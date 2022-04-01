#include "../umps/cp0.h"

/* ALGORITMO
    Determinare quale device/timer tra quelli in attesa ha priorità piu' alta (Cause.IP(8 bit) linee in cui ci sono interrupt in attesa)
    * ottengo registro Cause
    * accedo a campo IP
    * scorro Cause.IP partendo da linea piu' bassa 
        ? ho 8 device per ogni linea?
        scorro i device della linea        

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
*/

void interruptHandler()
{
  //int cause_reg = getCAUSE(); // accedo a registro Cause
  //cause_reg = cause_reg && CAUSE_IP_MASK;  // estraggo campo IP tramite AND bit a bit 

  for(int i=1; i<8; i++)  // scorro le 7 linee (linea 0 è da ignorare)
  {
    if(CAUSE_IP(i) == 1)  // se bit di i-esima linea è 1 c'è un interrupt pending
    {
      switch(i)
      {
        case 0:
          PANIC();
        break;

        case 1:
          PLTTimerInterrupt(i);
        break;

        case 2:
          intervalTimerInterrupt(i);
        break;

        // ? se non metto il case fa in automatico questo per i>2?
        nonTimerInterrupt(i);
      }
    }
  }
}

void PLTTimerInterrupt(int line)
{

}

void intervalTimerInterrupt(int line)
{

}

void nonTimerInterrupt(int line)
{

}

// ! PORCODIO 
// TODO
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
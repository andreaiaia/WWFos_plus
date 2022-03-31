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

void interrupt_handler()
{
    int cause_reg = getCAUSE(); // accedo a registro Cause
    cause_reg = cause_reg && CAUSE_IP_MASK;  // estraggo campo IP tramite AND bit a bit 

    for(int i=1; i<8; i++)  // scorro le 7 linee (linea 0 è da ignorare)
    {
        if(CAUSE_IP(line) == 1)  // ? verifico se il bit della i-esima linea è a 1 ovvero c'è un interrupt in attesa
        {
            
        }
    }
}
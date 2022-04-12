    // * Exception code 0 -> Si passa il controllo al device interrupt handler
    // * Exception code 1-3 -> Si passa il controllo al TLB exception handler
    // * Exception code 4-7 9-12 -> Si passa il controllo al Program Trap exception handler
    // * Exception code 8 -> Si passa il controllo al SYSCALL exception handler
    
#include "exception.h"

void exceptionHandler() {
    if (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) == 0)
    {
        interruptHandler();
    }
    else if ((CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) >= 1) && (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) <= 3))
    {
        PassUpOrDie(PGFAULTEXCEPT);
    }
    else if (((CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) >= 4) && (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) <= 7)) || ((CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) >= 9) && (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) <= 12)))
    {
        PassUpOrDie(GENERALEXCEPT);
    }
    else if (CAUSE_GET_EXCCODE(STATO_PROCESSO->cause) == 8)
    {
        syscallExceptionHandler(STATO_PROCESSO->reg_a0);        
    }
}
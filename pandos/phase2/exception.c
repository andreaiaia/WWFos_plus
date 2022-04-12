    // * Exception code 0 -> Si passa il controllo al device interrupt handler
    // * Exception code 1-3 -> Si passa il controllo al TLB exception handler
    // * Exception code 4-7 9-12 -> Si passa il controllo al Program Trap exception handler
    // * Exception code 8 -> Si passa il controllo al SYSCALL exception handler
    
#include "exception.h"

void exceptionHandler() {
    if (DECODED_EXCEPTION_CAUSE == 0)
    {
        interruptHandler();
    }
    else if ((DECODED_EXCEPTION_CAUSE >= 1) && (DECODED_EXCEPTION_CAUSE <= 3))
    {
        PassUpOrDie(PGFAULTEXCEPT);
    }
    else if (((DECODED_EXCEPTION_CAUSE >= 4) && (DECODED_EXCEPTION_CAUSE <= 7)) || ((DECODED_EXCEPTION_CAUSE >= 9) && (DECODED_EXCEPTION_CAUSE <= 12)))
    {
        PassUpOrDie(GENERALEXCEPT);
    }
    else if (DECODED_EXCEPTION_CAUSE == 8)
    {
        syscallExceptionHandler(REG_A0_ST);        
    }
}
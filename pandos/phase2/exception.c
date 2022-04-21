#include "exception.h"

void exceptionHandler()
{
    klog_print("EXH - Entro\n");
    // * Exception code 0 -> Si passa il controllo al device interrupt handler
    if (DECODED_EXCEPTION_CAUSE == 0)
    {
        klog_print("EXH_INT - Interrupt handler\n");
        interruptHandler();
    }
    // * Exception code 1-3 -> Si passa il controllo al TLB exception handler (PassUpOrDie PGFAULTEXCEPT)
    else if ((DECODED_EXCEPTION_CAUSE >= 1) && (DECODED_EXCEPTION_CAUSE <= 3))
    {
        klog_print("EXH_TLB - PassUpOrDie(1-3)\n");
        PassUpOrDie(PGFAULTEXCEPT);
    }
    // * Exception code 4-7 9-12 -> Si passa il controllo al Program Trap exception handler (PassUpOrDie GENERALEXCEPT)
    else if (((DECODED_EXCEPTION_CAUSE >= 4) && (DECODED_EXCEPTION_CAUSE <= 7)) || ((DECODED_EXCEPTION_CAUSE >= 9) && (DECODED_EXCEPTION_CAUSE <= 12)))
    {
        klog_print("EXH_TRAP: ");
        klog_print_hex(DECODED_EXCEPTION_CAUSE);
        klog_print("\n");
        PassUpOrDie(GENERALEXCEPT);
    }
    // * Exception code 8 -> Si passa il controllo al SYSCALL exception handler
    else if (DECODED_EXCEPTION_CAUSE == 8)
    {
        klog_print("EXH_SYSCALL\n");
        syscallExceptionHandler(REG_A0_SS);
    }
}
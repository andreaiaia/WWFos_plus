#include "exception.h"

void exceptionHandler()
{
    // Exception code 0 -> Si passa il controllo al device interrupt handler
    if (DECODED_EXCEPTION_CAUSE == 0)
        interruptHandler();
    // Exception code 1-3 -> Si passa il controllo al TLB exception handler (PassUpOrDie PGFAULTEXCEPT)
    else if ((DECODED_EXCEPTION_CAUSE >= 1) && (DECODED_EXCEPTION_CAUSE <= 3))
    {
        PassUpOrDie(PGFAULTEXCEPT);
        scheduler();
    }
    // Exception code 4-7 9-12 -> Si passa il controllo al Program Trap exception handler (PassUpOrDie GENERALEXCEPT)
    else if (((DECODED_EXCEPTION_CAUSE >= 4) && (DECODED_EXCEPTION_CAUSE <= 7)) || ((DECODED_EXCEPTION_CAUSE >= 9) && (DECODED_EXCEPTION_CAUSE <= 12)))
    {
        PassUpOrDie(GENERALEXCEPT);
        scheduler();
    }
    // Exception code 8 -> Si passa il controllo al SYSCALL exception handler
    else if (DECODED_EXCEPTION_CAUSE == 8)
        syscallExceptionHandler(REG_A0_SS);
}

void extern klog_print_hex();

void uTLB_RefillHandler()
{
    klog_print("refill handler\n");
    unsigned int vpn = PROCESSOR_SAVED_STATE->entry_hi >> VPNSHIFT;

    pteEntry_t *table = current_p->p_supportStruct->sup_privatePgTbl;
    unsigned int newEntryHI, newEntryLO;

    for (int i = 0; i < USERPGTBLSIZE; i++)
    {
        if (table[i].pte_entryHI >> VPNSHIFT == vpn)
        {
            newEntryHI = table[i].pte_entryHI;
            newEntryLO = table[i].pte_entryLO;
            klog_print("Trovata pagina: ");
            klog_print_hex(i);
            break;
        }
    }
    klog_print("refill handler - post for");

    // Aggiungo la PTE nel TLB
    setENTRYHI(newEntryHI);
    setENTRYLO(newEntryLO);
    TLBWR();

    LDST(PROCESSOR_SAVED_STATE);
}
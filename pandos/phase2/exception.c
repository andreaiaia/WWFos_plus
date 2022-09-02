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

void uTLB_RefillHandler()
{
    size_t index = getPTEIndex(PROCESSOR_SAVED_STATE->entry_hi);
    pteEntry_t pte = current_p->p_supportStruct->sup_privatePgTbl[index];

    // Aggiungo la PTE nel TLB
    setENTRYHI(pte.pte_entryHI);
    setENTRYLO(pte.pte_entryLO);
    TLBWR();

    LDST(PROCESSOR_SAVED_STATE);
}

size_t getPTEIndex(memaddr entry_hi)
{
    // processo: prendi EntryHI e ricavi il VPN, poi dal VPN ricavi l'index
    size_t vpn = entryhi >> VPNSHIFT;
    size_t index = vpn - (KUSEG >> VPNSHIFT);

    return index;
}
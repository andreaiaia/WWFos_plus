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
    // int index = ENTRYHI_GET_ASID(PROCESSOR_SAVED_STATE->entry_hi);
    // EntryHI to VPN
    unsigned int vpn = PROCESSOR_SAVED_STATE->entry_hi >> VPNSHIFT;
    // VPN to Index
    int i;
    for (i = 0; i < MAXPAGES; i++)
    {
        pteEntry_t pte = current_p->p_supportStruct->sup_privatePgTbl[i];
        if ((pte->entry_hi >> VPNSHIFT) == vpn)
            break;
    }

    pteEntry_t pte = current_p->p_supportStruct->sup_privatePgTbl[i];

    // setENTRYHI(pte.pte_entryHI);
    current_p->p_supportStruct->sup_privatePgTbl->pte_entryHI = pte.pte_entryHI;
    TLBP();
    if (getINDEX() & PRESENTFLAG)
    {
        // Aggiungo la PTE nel TLB
        setENTRYHI(pte.pte_entryHI);
        setENTRYLO(pte.pte_entryLO);
        TLBWR();
    }
    klog_print("carico stato dopo tlb\n");
    LDST(PROCESSOR_SAVED_STATE);
}
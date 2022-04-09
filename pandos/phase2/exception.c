#include "exception.h"
#define stato_processo ((state_t *)BIOSDATAPAGE)
//extern pcb_PTR current_p;
//#define PROCESSO_CORRENTE current_p
// CP0 Cause fields
// #define CAUSE_EXCCODE_MASK     0x0000007c
// #define CAUSE_EXCCODE_BIT      2
// #define CAUSE_GET_EXCCODE(x)   (((x) & CAUSE_EXCCODE_MASK) >> CAUSE_EXCCODE_BIT)

// ? chi chiama exceptionHandler() ? Dal manuale:
// ? Therefore, if the Pass Up Vector was correctly initialized, fooBar will be
// ? called (with a fresh stack) after each and every exception, exclusive of TLBRefill
// ? events.
// ? probabilmente lo fa grazie alla riga nel main quando diamo al passupvector l'indirizzo dell'excp handler

void exceptionHandler()
{
    // * Exception code 0 -> Si passa il controllo al device interrupt handler
    // * Exception code 1-3 -> Si passa il controllo al TLB exception handler
    // * Exception code 4-7 9-12 -> Si passa il controllo al Program Trap exception handler
    // * Exception code 8 -> Si passa il controllo al SYSCALL exception handler
    if (CAUSE_GET_EXCCODE(stato_processo->cause) == 0)
    {
        interruptHandler();
    }
    else if ((CAUSE_GET_EXCCODE(stato_processo->cause) >= 1) && (CAUSE_GET_EXCCODE(stato_processo->cause) <= 3))
    {
        uTLB_RefillHandler();
    }
    else if (((CAUSE_GET_EXCCODE(stato_processo->cause) >= 4) && (CAUSE_GET_EXCCODE(stato_processo->cause) <= 7)) || ((CAUSE_GET_EXCCODE(stato_processo->cause) >= 9) && (CAUSE_GET_EXCCODE(stato_processo->cause) <= 12)))
    {
        // TODO NICK LO FA
        // ! Program trap exception handler
    }
    else if (CAUSE_GET_EXCCODE(stato_processo->cause) == 8)
    {
        // !SYSCALL EXCEPTION HANDLER
        // !Io lo farei in SYSCALL_helpers.c, così da avere un vero "syscall exception handler" come da manuale 
        // !a pagina 26 (9 del pdf)
        // ? Dal manuale abbiamo:
        // ? Furthermore, the processor state at the time of the exception
        // ? (the saved exception state) will have been stored (for Processor 0) at the start
        // ? of the BIOS Data Page (0x0FFF.F000).
        // TODO se il processo che fa una syscall è in kernel mode E
        // TODO a0 contiene un numero negativo, syscall, altrimenti i guess termina

        // TODO FARE SOLO CONTROLLO SE IN KERNEL MODE E SE SYSCALL (A0 NEGATIVO) POI PASSARE CONTROLLO A
        // TODO syscallExceptionHandler(); da mettere in SYSCALL.c
        if (PROCESSO_CORRENTE->p_s.status == STATUS_KUp) {

        }
        
        switch(stato_processo->reg_a0) { //so che non funziona, è un placeholder mi serve capire come prendere lo stato del processo chiamante
            case -1:
            Create_Process(stato_processo->reg_a1, stato_processo->reg_a2, stato_processo->reg_a3);
            //? chiamata allo scheduler?
            break;

        }
    }
}
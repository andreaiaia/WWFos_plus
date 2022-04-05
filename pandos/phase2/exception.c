#include "exception.h"

// CP0 Cause fields
// #define CAUSE_EXCCODE_MASK     0x0000007c
// #define CAUSE_EXCCODE_BIT      2
// #define CAUSE_GET_EXCCODE(x)   (((x) & CAUSE_EXCCODE_MASK) >> CAUSE_EXCCODE_BIT)
// ! L'assunto è che CAUSE_GET_EXCCODE restituisca l'exception code relativo al registro
// ! cause passato. Quindi gli passo causa che contiene il registro cause della cpu0 ottenuto da getCause();
void exceptionHandler() {
    // * Exception code 0 -> Si passa il controllo al device interrupt handler
    // * Exception code 1-3 -> Si passa il controllo al TLB exception handler
    // * Exception code 4-7 9-12 -> Si passa il controllo al Program Trap exception handler
    // * Exception code 8 -> Si passa il controllo al SYSCALL exception handler
    memaddr causa = getCause();
    if (CAUSE_GET_EXCCODE(causa) == 0) {
        interruptHandler();
    } 
    else if ((CAUSE_GET_EXCCODE(causa) >= 1) && (CAUSE_GET_EXCCODE(causa) <= 3)) {
        uTLB_RefillHandler(); 
    } 
    else if (((CAUSE_GET_EXCCODE(causa) >= 4) && (CAUSE_GET_EXCCODE(causa) <= 7)) || ((CAUSE_GET_EXCCODE(causa) >= 9) && (CAUSE_GET_EXCCODE(causa) <= 12)))  {
        // ! Program trap exception handler
    }
    else if (CAUSE_GET_EXCCODE(causa) == 8) {
        // !SYSCALL EXCEPTION HANDLER
    }


    
}
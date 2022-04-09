#include "exception.h"

// CP0 Cause fields
// #define CAUSE_EXCCODE_MASK     0x0000007c
// #define CAUSE_EXCCODE_BIT      2
// #define CAUSE_GET_EXCCODE(x)   (((x) & CAUSE_EXCCODE_MASK) >> CAUSE_EXCCODE_BIT)

void exceptionHandler()
{
    // * Exception code 0 -> Si passa il controllo al device interrupt handler
    // * Exception code 1-3 -> Si passa il controllo al TLB exception handler
    // * Exception code 4-7 9-12 -> Si passa il controllo al Program Trap exception handler
    // * Exception code 8 -> Si passa il controllo al SYSCALL exception handler
    if (CAUSE_GET_EXCCODE(((state_t *)BIOSDATAPAGE)->cause) == 0)
    {
        interruptHandler();
    }
    else if ((CAUSE_GET_EXCCODE(((state_t *)BIOSDATAPAGE)->cause) >= 1) && (CAUSE_GET_EXCCODE(((state_t *)BIOSDATAPAGE)->cause) <= 3))
    {
        uTLB_RefillHandler();
    }
    else if (((CAUSE_GET_EXCCODE(((state_t *)BIOSDATAPAGE)->cause) >= 4) && (CAUSE_GET_EXCCODE(((state_t *)BIOSDATAPAGE)->cause) <= 7)) || ((CAUSE_GET_EXCCODE(((state_t *)BIOSDATAPAGE)->cause) >= 9) && (CAUSE_GET_EXCCODE(((state_t *)BIOSDATAPAGE)->cause) <= 12)))
    {
        // ! Program trap exception handler
    }
    else if (CAUSE_GET_EXCCODE(((state_t *)BIOSDATAPAGE)->cause) == 8)
    {
        // !SYSCALL EXCEPTION HANDLER
        // !Io lo farei in SYSCALL_helpers.c, così da avere un vero "syscall exception handler" come da manuale 
        // !a pagina 26 (9 del pdf)
        // ? se il processo che fa una syscall è in kernel mode E
        // ? a0 contiene un numero negativo, syscall, altrimenti i guess termina
        // ? dove trovo lo stato del processo che attualmente stiamo esaminando? 
        // ? non mi viene passato da nessuno come parametro
        switch(state_t->reg_a0) { //so che non funziona, è un placeholder
            case -1:
            Create_Process();
            //? chiamata allo scheduler?
            break;


        }
    }
}
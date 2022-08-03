#include "vmSupport.h"

// Questo modulo implementa il TLB exception handler
// e le funzioni per leggere e scrivere i dispositivi flash
// implementa anche Swap Pool e Swap pool table e una funzione
// initSwapStructs da richiamare poi in initProc.c

// ! Attenzione: il TLB-refill handler andrebbe implementato
// ! dove avevamo messo il placeholder (phase2/exceptions.c)
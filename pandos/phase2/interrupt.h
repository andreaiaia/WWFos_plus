#include "main.h"
#include "SYSCALL_helpers.h"

void interruptHandler();
void PLTTimerInterrupt(int line);      // 3.6.2 pandos
void intervalTimerInterrupt(int line); // 3.6.3 pandos
void nonTimerInterrupt(int line);      // 3.6.1 pandos

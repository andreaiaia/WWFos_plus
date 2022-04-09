/*#include <umps3/umps/libumps.h>
#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"
#include "main.h"
#include "scheduler.h"
#include "SYSCALL_helpers.h"
*/
#include <umps3/umps/libumps.h>
#include <umps3/umps/arch.h>
#include <umps3/umps/cp0.h>
#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"
#include "main.h"
#include "scheduler.h"
#include "SYSCALL_helpers.h"


void interruptHandler();
void PLTTimerInterrupt(int line);   // 3.6.2 pandos
void intervalTimerInterrupt(int line);  // 3.6.3 pandos
void nonTimerInterrupt(int line); // 3.6.1 pandos

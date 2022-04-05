#include <umps3/umps/libumps.h>
#include "../h/listx.h"
#include "../h/pandos_types.h"
#include "../h/pandos_const.h"
#include "../phase1/asl.h"
#include "../phase1/pcb.h"
#include "main.h"
#include "scheduler.h"
#include "SYSCALL.h"

// Helper collegati alla Terminate_Process
void Exterminate(pcb_PTR process);
pcb_PTR FindProcess(int pid);
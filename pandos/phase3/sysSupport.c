// Qui vanno implementati il general exception handler (sezione 4.6)
// il SYSCALL exception handler (sezione 4.7) e il program trap exception handler (sezione 4.8)

void trapExcHandler() {

}

//? docs: 4.7.2 (SYS2 - Terminate)
/*
   implementa [SYS2]:  void SYSCALL(TERMINATE, 0, 0, 0)
   wrapper della NSYS2, killa user process

 */
//! per richiedere questa syscall è necessario inserire il valore 2 nel registro a0 e poi chiamare la syscall
void Terminate(int pid) {
    Terminate_Process(pid);
}
// Qui vanno implementati il general exception handler (sezione 4.6)
// il SYSCALL exception handler (sezione 4.7) e il program trap exception handler (sezione 4.8)


// wrapper della NSYS2, killa user process
void Trap_Exc_Handler() //? ho messo void ma non ne sono certo, non vedo cosa dovrebbe tornare in ogni caso visto che deve solo UCCIDERE
{
    //? docs: 4.8

    // se processo in mutua esclusione prima libero il semaforo
    if(current_p->semAdd != 0) {  //? non sono affatto sicuro di come controllare se è in mutua esclusione
        Verhogen(current_p->semAdd);
        Terminate_Process(&(current_p->p_pid));
    }
    else{
        Terminate_Process(&(current_p->p_pid));
    }
    //? alla fine va chiamato scheduler()?
}

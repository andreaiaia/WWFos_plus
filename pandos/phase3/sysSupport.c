// Qui vanno implementati il general exception handler (sezione 4.6)
// il SYSCALL exception handler (sezione 4.7) e il program trap exception handler (sezione 4.8)

extern pcb_PTR current_p;


void trapExceptionHandler() //? ho messo void ma non ne sono certo, non vedo cosa dovrebbe tornare in ogni caso visto che deve solo UCCIDERE
{
    //? docs: 4.8

    // se processo in mutua esclusione prima libero il semaforo
    //! usare semaforo del support level non quello del livello 3
    //! mettere il valore 2 in a0
    //! la terminologia da usare è del tipo SYSCALL (TERMINATE, 0, 0, 0)
    if(semaforoLvlSupport != 0) {  //? non sono affatto sicuro di come controllare se è in mutua esclusione
        Verhogen(current_p->semAdd);
        Terminate_Process(&(current_p->p_pid));
    }
    else{
        Terminate_Process(&(current_p->p_pid));
    }
    //? alla fine va chiamato scheduler()?
}

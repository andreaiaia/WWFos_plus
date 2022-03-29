void fooBar()
{
    // riferimenti a capitolo 3.4 della guida Pandos
    // fooBar dovrebbe essere il Kernel exception handler
    // e viene chiamato ad ogni exception tranne per le TLB-refill
    // il processor_state al momento dell'eccezione viene memorizzato all'inizio
    // del BIOS Data Page (0x0FFF.F000)
    // la causa dell'eccezione invece è memorizzata nei bit 2..6
    // del registro Cause (il campo ExcCode sono i bit 2..6)
    // ? l'idea qui era di estrarre quei bit per avere la causa dell'eccezione
    // ? visto che a seconda del codice d'eccezione bisogna affidare a uno specifico
    // ? handler la situazione
    int exc_code = CAUSE_GET_EXCCODE(CP0_Cause) // vedi cp0 file
}
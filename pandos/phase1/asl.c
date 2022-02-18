#include "asl.h"
#include "pcb.h"
// Tabella dei semafori di dimensione massima MAXPROC (allocazione in memoria dei semafori)
static semd_t semd_table[MAXPROC];
// Lista dei semafori liberi/inutilizzati
static LIST_HEAD(semdFree_h);
// Lista dei semafori attivi/utilizzati (ASL)
static LIST_HEAD(semd_h);

/*
    14. 
    Inserisce il pcb puntato da p nella coda dei 
    processi bloccati associata al semaforo
    avente chiave semAdd.

    Se il semaforo corrispondente non è
    presente nella ASL, alloca un nuovo SEMD dalla
    lista di quelli liberi (semdFree) e lo inserisce nella
    ASL, settando I campi in maniera opportuna. 
    
    Return: 1 se semdFree_h è vuota, 0 altrimenti.

    author: -W
*/
int insertBlocked(int *semAdd, pcb_t *p) {
    semd_PTR tmp = NULL;
    /* scorre la lista dei semafori attivi/utilizzati */
    list_for_each_entry(tmp, &semd_h, s_link){
        if ((tmp->s_key == semAdd)) {
            //p->p_semAdd = tmp->s_key;
            p->p_semAdd = semAdd;
            *semAdd = 0;
            list_add_tail(&(p->p_list), &(tmp->s_procq));
            addokbuf("trovato semaforo con semadd nella asl  \n");
            return FALSE;
        }
    }
    //Caso in cui il semaforo non è presente nella ASL
    //return true se non ci sono semafori liberi da allocare
    if (list_empty(&semdFree_h)) return TRUE;
    //allocazione nuovo semd dalla lista semdFree
    semd_PTR semallocato = container_of(list_next(&semdFree_h), semd_t, s_link);
    list_del(&(semallocato->s_link));
    semallocato->s_key = semAdd;
    *semAdd = 0;
    list_add_tail(&(semallocato->s_link), &semd_h); //inserisce il semaforo nella ASL se non in coda si arrabbia
    list_add_tail(&(p->p_list), &(semallocato->s_procq));  //se non aggiungo in coda si rompe la headblocekd
    addokbuf("allocato semaforo con chiave semadd  \n");
    return FALSE;
}


/*
    15. 
    Rimuove il primo PCB dalla coda dei processi bloccati (s_procq) associata al 
    SEMD della ASL con chiave semAdd. 
    Se la coda dei processi bloccati per il semaforo diventa vuota, 
    rimuove il descrittore corrispondente dalla ASL 
    e lo inserisce nella coda dei descrittori liberi (semdFree_h). 
    
    semAdd: chiave del semd
    
    Return: Puntatore al pcb rimosso || NULL.
*/   
pcb_t *removeBlocked(int *semAdd) {
    semd_PTR s_iter;
    pcb_PTR res = NULL;
    list_for_each_entry(s_iter, &semd_h, s_link) {  
        if (s_iter->s_key == semAdd) {
            //pcb_PTR tmp = container_of(list_next(&s_iter->s_procq), pcb_t, p_list);
            //res = tmp;
            res = removeProcQ(&(s_iter->s_procq)); //risultato diverso chiedere al tutor
            //list_del(&(tmp->p_list)); //risultato diverso chiedere al tutor
            if (list_empty(&(s_iter->s_procq))) {
                 list_del(&(s_iter->s_link));
                 list_add(&(s_iter->s_link), &semdFree_h);
            }
            return res;
        }  
    }
    return NULL;   
}
/*
    16. 
    Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato (p->p_semAdd).
    Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il semd corrispondente da semd_h e 
    lo inserisce in semdFree_h (lo rimette nella coda dei semafori liberi).
    
    p: puntatore a PCB da rimuovere.
    
    Return: p, puntatore al PCB rimosso || NULL se il PCB non compare nella coda (stato di errore).
*/
pcb_t *outBlocked(pcb_t *p) {
    semd_PTR sem_iteratore;
    pcb_PTR pcb_iteratore;
    list_for_each_entry(sem_iteratore, &semd_h, s_link){
        outProcQ(&(sem_iteratore->s_procq), p);
        if (list_empty(&(sem_iteratore->s_procq))) {
            sem_iteratore->s_key=NULL;
            list_del(&(sem_iteratore->s_link));
            list_add_tail(&(sem_iteratore->s_link), &semdFree_h );
        }
        addokbuf("returno p  \n");
        return p;
    }   
    // Stato di errore
    return NULL;
}

/*
    17. 
    Restituisce (senza rimuoverlo) il puntatore al primo PCB della coda dei processi associata al SEMD
    con chiave semAdd.
    
    semAdd: chiave del SEMD.
    
    Return: puntatore al primo pcb_t nella coda dei processi || NULL se non c'è il SEMD o se la sua coda è vuota.
*/
pcb_t *headBlocked(int *semAdd) {
    semd_PTR sem_iteratore;
    list_for_each_entry(sem_iteratore, &semd_h, s_link){
        if ((sem_iteratore->s_key) == semAdd){
            if (list_empty(&(sem_iteratore->s_procq))) {
                    addokbuf("riga 124\n");
                    return NULL; // La coda dei pcb è vuota
            }
            addokbuf("sono passato in una headblocked\n");
            return container_of(list_next(&(sem_iteratore->s_procq)), pcb_t, p_list);
        }
    }
    return NULL;
}

/*
    18. 
    Inizializza la lista semdFree in modo da contenere tutti gli elementi della semdTable.
    Questo metodo viene invocato una volta sola durante l'inizializzazione dei dati.
*/
void initASL() {
    //Ciclo ogni elemento della tabella dei semafori e lo aggiungo alla lista dei semafori liberi usando la macro del kernel linux list_add.
    for (int i = 0; i < MAXPROC; i++) {
        list_add(&(semd_table[i].s_link), &semdFree_h);
        semd_table[i].s_key = NULL;    
        INIT_LIST_HEAD(&(semd_table[i].s_procq));
    }
}
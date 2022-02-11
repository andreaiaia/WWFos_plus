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
    int flag = 0;
    /* scorre la lista dei semafori attivi/utilizzati */
    list_for_each_entry(tmp, &semd_h, s_link){
        if ((tmp->s_key == semAdd) && (flag==0)) {
            p->p_semAdd = tmp->s_key;
            *(tmp->s_key) = *(tmp->s_key) + 1;
            list_add_tail(&(p->p_list), &(tmp->s_procq));
            flag=1;
            return FALSE;
        }
    }
    //Caso in cui il semaforo non è presente nella ASL
    if (flag == 0) {
        //return true se non ci sono semafori liberi da allocare
        if (list_empty(&semdFree_h)) return TRUE;
        //allocazione nuovo semd dalla lista semdFree
        semd_PTR semallocato = container_of(list_next(&semdFree_h), semd_t, s_link);
        list_del(list_next(&semdFree_h));
        //INIT_LIST_HEAD(&semallocato->s_procq); fixato in ASL
        semallocato->s_key = semAdd;
        *(semallocato->s_key) = 1;
        list_add_tail(&(semallocato->s_link), &semd_h);
        return FALSE;
    }
    return FALSE;
}


/*
    15. 
    Rimuove il primo PCB dalla coda dei processi bloccati (s_procq) associata al SEMD della ASL con chiave semAdd. 
    Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il descrittore corrispondente dalla ASL 
    e lo inserisce nella coda dei descrittori liberi (semdFree_h). 
    
    semAdd: chiave del semd
    
    Return: Puntatore al pcb rimosso || NULL.
*/   
pcb_t *removeBlocked(int *semAdd) {
    semd_PTR s_iter;
    pcb_PTR res = NULL;
    list_for_each_entry(s_iter, &semd_h, s_link) {  
        if (s_iter->s_key == semAdd) {
            //pcb_PTR tmp = container_of(&(s_iter.s_procq)->next), pcb_t, p_list);
            res = removeProcQ(&(s_iter->s_procq));
            if (emptyProcQ(&(s_iter->s_procq))){
                 list_del(&(s_iter->s_link));
                 list_add(&(s_iter->s_link), &semdFree_h);
            }
        return res;
        }  
    }
    return NULL;   
}

/*
pcb_t *removeBlocked(int *semAdd) {
    struct semd_t *s_iter;
    list_for_each_entry(s_iter, &semd_h, s_link) {   // scorro ASL
        if (s_iter->s_key == semAdd) {
            if (list_empty( &(s_iter->s_procq) )) return NULL; // La coda dei pcb è vuota
    
            struct pcb_t *p = container_of( &(s_iter->s_procq), pcb_t, p_list);

            list_del((s_iter->s_procq.next) );  // tolgo pcb trovato da s_procq
            if(list_empty( &(s_iter->s_procq) ) == 1){    // se s_procq diventa vuota
                list_del( &(p->p_list) );
                list_add( &(p->p_list), &semdFree_h ); 
            }
            return p;
        }
    }
    // Non esiste il semd
    return NULL;
}    */
/*
    16. 
    Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato (p->p_semAdd).
    Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il semd corrispondente da semd_h e lo inserisce in semdFree_h (lo rimette nella coda dei semafori liberi).
    
    p: puntatore a PCB da rimuovere.
    
    Return: p, puntatore al PCB rimosso || NULL se il PCB non compare nella coda (stato di errore).
*/
pcb_t *outBlocked(pcb_t *p) {
    struct semd_t *s_iter;
    // Questo ciclo scorre la lista dei semafori (semd_h) e ad ogni iterata s_iter punta al semd_t corrente
    list_for_each_entry(s_iter, &semd_h, s_link) {
        // Controllo di aver trovato il semd corretto confrontando le chiavi
        if (s_iter->s_key == p->p_semAdd) {
            struct pcb_t *p_iter;
            /*
            Questo ciclo scorre nella lista dei processi bloccati per trovare il pcb_t
            che ci interessa. Ad ogni iterata p_iter punta al pcb_t corrente.
            */
            list_for_each_entry(p_iter, &(s_iter->s_procq), p_list) {
                if (p_iter == p) {
                    list_del(&(p_iter->p_list));
                    (*(s_iter->s_key))--;
                    /* 
                    Se il pcb rimosso era l'unico, il semd diventa libero e viene tolto dalla lista 
                    dei semd attivi e messo in quella dei semd liberi .
                    */
                    if (*(s_iter->s_key) == 0) {
                        list_del( &(s_iter->s_link) );
                        list_add( &(s_iter->s_link), &semdFree_h );
                    }

                    return p;
                }
            }
            /*
            Return non strettamente necessario, l'ho messo solo per terminare prima l'esecuzione nel caso in cui
            il semaforo non contenga un p_iter == p, in tal caso non ha senso continuare il ciclo.
            */
            return NULL; 
        }
    }
    // Stato di errore
    return NULL;
}

/*
    17. 
    Restituisce (senza rimuoverlo) il puntatore al primo PCB della coda dei processi associata al SEMD con chiave semAdd.
    
    semAdd: chiave del SEMD.
    
    Return: puntatore al primo pcb_t nella coda dei processi || NULL se non c'è il SEMD o se la sua coda è vuota.
*/
pcb_t *headBlocked(int *semAdd) {
    struct semd_t *s_iter;
    list_for_each_entry(s_iter, &semd_h, s_link) {
        if (s_iter->s_key == semAdd) {
            if (list_empty( &(s_iter->s_procq) )) return NULL; // La coda dei pcb è vuota

            // Uso la macro container_of per prendere il primo pcb della coda e restituirlo
            struct pcb_t *p = container_of( &(s_iter->s_procq), pcb_t, p_list );
            return p;
        }
    }
    // Non esiste il semd
    return NULL;
}

/*
    18. 
    Inizializza la lista semdFree in modo da contenere tutti gli elementi della semdTable.
    Questo metodo viene invocato una volta sola durante l'inizializzazione dei dati.
*/
void initASL() {
    INIT_LIST_HEAD(&semdFree_h);
    // Controllo per sicurezza, se la lista dei semafori liberi non è vuota la funzione termina
    if (!list_empty(&semdFree_h)) return;

    //Ciclo ogni elemento della tabella dei semafori e lo aggiungo alla lista dei semafori liberi usando la macro del kernel linux list_add.
    for (int i = 0; i < MAXPROC; i++) {
        list_add( &(semd_table[i].s_link), &semdFree_h );
        INIT_LIST_HEAD(&(semd_table[i].s_procq));
    }
/*   
	INIT_LIST_HEAD(&semdFree_h);
	for (int i = 0; i < MAXPROC; i++)
	{
		semd_t sem = semd_table[i];
		INIT_LIST_HEAD(&(sem.s_link));
		sem.s_key = NULL;    
		INIT_LIST_HEAD(&(sem.s_procq));
        list_add(&(sem.s_link), &semdFree_h);
	}
*/
}
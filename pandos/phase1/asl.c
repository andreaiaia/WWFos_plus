#include "asl.h"

// Tabella dei semafori di dimensione massima MAXPROC (allocazione in memoria dei semafori)
static semd_t semd_table[MAXPROC];
// Lista dei semafori liberi/inutilizzati
static LIST_HEAD(semdFree_h);
// Lista dei semafori attivi/utilizzati
static LIST_HEAD(semd_h);

/*
    14. Viene inserito il PCB puntato da p nella coda dei
    processi bloccati associata al SEMD con chiave
    semAdd. Se il semaforo corrispondente non è
    presente nella ASL, alloca un nuovo SEMD dalla
    lista di quelli liberi (semdFree) e lo inserisce nella
    ASL, settando I campi in maniera opportuna (i.e.
    key e s_procQ). Se non è possibile allocare un
    nuovo SEMD perché la lista di quelli liberi è vuota,
    restituisce TRUE. In tutti gli altri casi, restituisce
    FALSE

*/

int insertBlocked(int *semAdd, pcb_t *p) {





}


/*
    15. Rimuove il primo PCB dalla coda dei processi bloccati (s_procq) associata al SEMD della ASL con chiave semAdd. Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il descrittore corrispondente dalla ASL e lo inserisce nella coda dei descrittori liberi (semdFree_h). 
    @param: chiave del semd
    Return: Puntatore al pcb rimosso
            or NULL.
*/   
pcb_t *removeBlocked(int *semAdd) {
    struct semd_t *s_iter;
    list_for_each_entry(s_iter, &semd_h, s_link) {   // scorro ASL
        if (s_iter->s_key == semAdd) {
            if (list_empty( &(s_iter->s_procq) )) return NULL; // La coda dei pcb è vuota
    
            struct pcb_t *p = container_of( &(s_iter->s_procq), pcb_t, p_list);

            list_del( &(s_iter->s_procq->next) );  // tolgo pcb trovato da s_procq
            if(list_empty( &(s_iter->s_procq) ) == 1){    // se s_procq diventa vuota
                list_del( p->p_list );
                list_add( p->p_list, &semdFree_h );
            }
            return p;
        }
    }
    // Non esiste il semd
    return NULL;
}    

/*
    16. Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato (p->p_semAdd).
    Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il semd corrispondente da semd_h e lo inserisce in semdFree_h (lo rimette nella coda dei semafori liberi).
    @param: puntatore a PCB da rimuovere.
    Return: p, puntatore al PCB rimosso
            or NULL se il PCB non compare nella coda (stato di errore).
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
    17. Restituisce (senza rimuoverlo) il puntatore al primo PCB della coda dei processi associata al SEMD con chiave semAdd.
    @param: chiave del SEMD.
    Return: puntatore al primo pcb_t nella coda dei processi
            or NULL se non c'è il SEMD o se la sua coda è vuota.
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
    18. Inizializza la lista semdFree in modo da contenere tutti gli elementi della semdTable.
    Questo metodo viene invocato una volta sola durante l'inizializzazione dei dati.
*/
void initASL() {
    // Controllo per sicurezza, se la lista dei semafori liberi non è vuota la funzione termina
    if (!list_empty(&semdFree_h)) return;

    /* 
    Ciclo ogni elemento della tabella dei semafori e lo aggiungo alla lista dei semafori liberi usando la macro del kernel linux list_add.
    */
    for (int i = 0; i < MAXPROC; i++) {
        list_add( &(semd_table[i].s_procq), &semdFree_h );
    }
}
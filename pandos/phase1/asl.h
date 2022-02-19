#include "../h/pandos_const.h"
#include "../h/pandos_types.h"
#include "../h/listx.h"

// Gestione della ASL (Active Semaphore List)

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
*/
int insertBlocked(int *semAdd, pcb_t *p);

/*
    15. Ritorna il primo PCB dalla coda dei processi bloccati (s_procq) associata al SEMD della ASL con chiave semAdd. 
    Se tale descrittore non esiste nella ASL, restituisce NULL. Altrimenti, restituisce l’elemento rimosso. 
    Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il descrittore
    corrispondente dalla ASL e lo inserisce nella coda dei descrittori liberi (semdFree_h). 
    @param: 
    Return: 
*/
pcb_t *removeBlocked(int *semAdd);

/*
    16. 
    Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato (p->p_semAdd).
    Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il semd corrispondente da semd_h e lo inserisce in semdFree_h (lo rimette nella coda dei semafori liberi).
    
    p: puntatore a PCB da rimuovere.
    
    Return: p, puntatore al PCB rimosso || NULL se il PCB non compare nella coda (stato di errore).
*/
pcb_t *outBlocked(pcb_t *p);

/*
    17. 
    Restituisce (senza rimuoverlo) il puntatore al primo PCB della coda dei processi associata al SEMD con chiave semAdd.
    semAdd: chiave del SEMD.
    
    Return: puntatore al primo pcb_t nella coda dei processi || NULL se non c'è il SEMD o se la sua coda è vuota.
*/
pcb_t *headBlocked(int *semAdd);

/*
    18. 
    Inizializza la lista semdFree in modo da contenere tutti gli elementi della semdTable.
    Questo metodo viene invocato una volta sola durante l'inizializzazione dei dati.
*/
void initASL();
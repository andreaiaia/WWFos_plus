#include "../h/pandos_const.h"
#include "../h/pandos_types.h"
#include "../h/listx.h"

// Allocation and deallocation of pcbs //

/* 
	1. Inizializza la lista pcbFree in modo da contenere tutti gli elementi della pcbFree_table. 
	Questo metodo deve essere chiamato una volta sola in fase di inizializzazione della struttura dati. 
*/
void initPcbs();

/*
	2. I pcb non più usati vengono restituiti alla lista pcbFree_h
	@param: puntatore al processo da aggiungere.
*/
void freePcb(pcb_t *);

/*
	3. Inizializza tutti i campi di un pcb togliendolo dalla lista pcbFree_h
	Return NULL se la lista pcbFree_h è vuota.
*/
pcb_t *allocPcb();

// Process Queue Maintenance //

/*
	4. Questa funzione crea una lista di PCB e la inizializza come vuota.
	@param: il puntatore alla testa della lista da creare.
*/
void mkEmptyProcQ(struct list_head *);

/* 
	5. Questa funzione return TRUE se la lista puntata da head è vuota
	false altrimenti
*/

int emptyProcQ(struct list_head *head);

/* 
	TODO: AGGIUNGERE DESCRIZIONE
*/
void insertProcQ(struct list_head *, pcb_t *);

/*
	TODO: AGGIUNGERE DESCRIZIONE
*/
pcb_t *headProcQ(struct list_head *):

/*
	8. Rimuove il primo elemento il cui puntatore a coda è puntato da tp dalla process queue.
	Se necessario aggiorna il puntatore di coda della process queue.
	Return: NULL if the process queue is empty
			or pointer to removed element.
*/
pcb_t *removeProcQ(struct list_head *);


// Process Tree Maintenance //

/* 
	10. RETURN TRUE se PCB puntato da P non ha figli, FALSE altrimenti.
*/

int emptyChild(pcb_t *p);

/*
	11. Inserisce il PCB puntato da p come figlio del PCB puntato da prnt.
	p = puntatore a pcb da inserire 
	prnt = parent da assegnare a p
*/

int insertChild(pcb_t *prnt, pcb_t *p);


/*
	12. Prende il primo child del pcb puntato da p non più un figlio di p.
	Return: NULL if initially there where no children of p
    		or pointer to the removed child.
*/
pcb_t * removeChild(pcb_t *);

/*
	13. Rimuove il PCB puntato da p dalla lista dei figli del padre.
	Se il PCB puntato da p non ha un padre, restituisce NULL, altrimenti restituisce l’elemento rimosso (cioè p).
	A differenza della removeChild, p può trovarsi in una posizione arbitraria (ossia non è necessariamente il primo figlio del padre).
	@author: Alex
*/
pcb_t * outChild(pcb_t * p);
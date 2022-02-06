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
	2. 
	Il pcb non più usato viene restituito alla lista pcbFree_h
	
	p: puntatore al processo da aggiungere a pcbFree_h.
*/
void freePcb(pcb_t *p);

/*
	3. 
	Prende un processo dalla lista pcbFree_h, 
	lo rimuove ed inizializza i suoi campi.

	Return: NULL se pcbFree_h vuota, pcb_t* altrimenti.
	
	author: -W
*/
pcb_t *allocPcb();

// Process Queue Maintenance //

/*
	4. 
	Questa funzione crea una lista di PCB e la inizializza come vuota.
	
	head: il puntatore alla testa della lista da creare.
*/
void mkEmptyProcQ(struct list_head *head);

/* 
	5.
	Verifica se una lista è vuota.

	head: lista da controllare

	return: 1 se la lista è vuota, 0 altrimenti

	author: -W
*/
int emptyProcQ(struct list_head *head);

/* 
	6. Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
*/
void insertProcQ(struct list_head *head, pcb_t *p);

/*
	7. Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO. Ritorna NULL se la coda non ha elementi.
*/
pcb_t *headProcQ(struct list_head *head);

/*
	8. 
	Rimuove il primo elemento il cui puntatore a coda è puntato da tp dalla process queue.
	Se necessario aggiorna il puntatore di coda della process queue.
	
	head: puntatore alla sentinella della coda procQ.
	
	Return: puntatore al PCB rimosso || NULL se la coda è vuota.
*/
pcb_t *removeProcQ(struct list_head *head);

/*
	9. 
	Rimuove il pcb puntato da p dalla coda 
	dei processi puntata da head. 
	
	head: puntatore alla testa della lista
	p: puntatore al PCB da rimuovere	

	Return: NULL se p non è presente nella coda.

	author: -W
*/
pcb_t* outProcQ(struct list_head* head, pcb_t *p);

// Process Tree Maintenance //

/*
	10.
	Verifica se il pcb passato ha figli.

	p: puntatore a pcb da testare.
	
	Return: 1 se non ha figli, 0 altrimenti.

	author: -W
*/
int emptyChild(pcb_t *p);


/*
	11. 
	Inserisce il pcb puntato da p come figlio del pcb puntato da prnt.
	
	p = puntatore a pcb da inserire 
	prnt = parent da assegnare a p

	author: -W
*/
void insertChild(pcb_t *prnt, pcb_t *p);


/*
	12. 
	Rende il primo child del pcb puntato da p non più un figlio di p.
	
	p: puntatore al pcb padre
	
	Return: puntatore al figlio rimosso || NULL se il pcb puntato da p non ha figli. 
*/
pcb_t * removeChild(pcb_t *p);

/*
	13. Rimuove il PCB puntato da p dalla lista dei figli del padre.
	Se il PCB puntato da p non ha un padre, restituisce NULL, altrimenti restituisce l’elemento rimosso (cioè p).
	A differenza della removeChild, p può trovarsi in una posizione arbitraria (ossia non è necessariamente il primo figlio del padre).
	@author: Alex
*/
pcb_t * outChild(pcb_t * p);
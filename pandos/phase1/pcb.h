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
*/
void freePcb(pcb_t *);


// Process Queue Maintenance //

/*
	Questa funzione crea una lista di PCB e la inizializza come vuota..
	@param: il puntatore alla testa della lista da creare.
*/
void mkEmptyProcQ(struct list_head *);

void insertProcQ(struct list_head *, pcb_t *);

pcb_t *headProcQ(struct list_head *):

/*
	Rimuove il primo elemento il cui puntatore a coda è puntato da tp dalla process queue.
	Se necessario aggiorna il puntatore di coda della process queue.
	Return: NULL if the process queue is empty
			or pointer to removed element.
*/
pcb_t *removeProcQ(pcb_t **);


// Process Tree Maintenance //

/*
	Prende il primo child del pcb puntato da p non più un figlio di p.
	Return: NULL if initially there where no children of p
    		or pointer to the removed child.
*/
pcb_t * removeChild(pcb_t *);

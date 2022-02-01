#include "pcb.h"

// Dichiarazione della lista dei pcb liberi
LIST_HEAD(pcbFree_h);

// Dichiarazione dell'array di pcb
pcb_t pcbFree_table[MAX_PROC];

/* 
	Viene eseguita solo all'inizio: prende tutti gli elementi della tabella pcbFree_table e li inserisce nella lista pcbFree_h
*/
void initPcbs() {
	if (list_empty(&(pcbFree_h))) {
		for (int i = 0; i < MAX_PROC; i++) {
			list_add( &(pcbFree_table[i].p_list), &pcbFree_h );
		}
	}
}

/*
	Questa funzione prende un processo e lo riaggiunge nella lista pcbFree_h dopo aver verificato che p non punti a NULL, fa uso della api del linux kernel per la gestione delle liste.
*/
void freePcb(pcb_t *p) {
	if (p != NULL) list_add( p, &pcbFree_h );
}

/*
	Questa funzione prende un processo dalla lista pcbFree_h, lo rimuove, inizializzando poi i suoi campi.
	Return del processo.
	Nel caso la lista pcbFree_h fosse vuota, return NULL.
*/

pcb_t *allocPcb() {
	if (list_empty(&(pcbFree_h))) {
		return(NULL);
	}
	else {
		struct list_head *elem = ((&pcbFree_h)->prev);
		(&pcbFree_h)->prev = (&pcbFree_h)->prev->prev;
		/* forse ha più senso farlo così
		list_del(elem);
		*/
		pcb_t *oggetto = container_of(elem, pcb_t, p_list);
		/* initializing process tree fields */
		oggetto->p_parent = NULL;
		INIT_LIST_HEAD(&(oggetto->p_child));
		INIT_LIST_HEAD(&(oggetto->p_sib));
		/* initializing process status information */
		/* campi della struct p_s di tipo state_t */
		oggetto->p_s.entry_hi=0;
		oggetto->p_s.cause=0;
		oggetto->p_s.status=0;
		oggetto->p_s.pc_epc=0;
		oggetto->p_s.gpr[STATE_GPR_LEN]=0;
		oggetto->p_s.hi=0;
		oggetto->p_s.lo=0;
		/* fine campi struct p_s di tipo state_t */
		oggetto->p_time = 0;
		/* initializing Pointer to the semaphore the process is currently blocked on */
		oggetto->p_semAdd = NULL;
		return(oggetto);
	} 
}

/*
	Questa funzione prende il puntatore passatogli e usa la macro del kernel linux per creare una lista di PCB vuota.
*/
void mkEmptyProcQ(struct list_head * head) {
	head = &(LIST_HEAD(procQ));
}

/*  
	Inserisce l’elemento puntato da p nella
	coda dei processi puntata da head.
*/
void insertProcQ(struct list_head *head, pcb_t *p) {
	list_add(&(p->p_list), head);
}

/*  
	Restituisce l’elemento di testa della coda dei processi da head,
	SENZA RIMUOVERLO. Ritorna NULL se la coda non ha elementi.
*/
pcb_t *headProcQ(struct list_head *head) {
  if (list_empty( &(pcbFree_h) ) {
		return NULL;
  }
}

#include "pcb.h"

// Dichiarazione dell'array di pcb (allocazione in memoria dei pcb)
static pcb_t pcbFree_table[MAXPROC];

// Dichiarazione della lista dei pcb liberi
static LIST_HEAD(pcbFree_h);

/*  
	1. Viene eseguita solo all'inizio: prende tutti gli elementi della tabella pcbFree_table e li inserisce nella lista pcbFree_h
	@author: Alex
*/
void initPcbs() {
	INIT_LIST_HEAD(&pcbFree_h); 
		for (int i = 0; i < MAXPROC; i++) {
			list_add( &(pcbFree_table[i].p_list), &pcbFree_h );
		}
}

/*
	2. Questa funzione prende un processo e lo riaggiunge nella lista pcbFree_h dopo aver verificato che p
	 non punti a NULL, fa uso della api del linux kernel per la gestione delle liste.
*/
void freePcb(pcb_t *p) {
	if (p != NULL) list_add( &(p->p_list), &pcbFree_h );
}

/*
	3. 
	Prende un processo dalla lista pcbFree_h, 
	lo rimuove ed inizializza i suoi campi.

	Return: NULL se pcbFree_h vuota, pcb_t* altrimenti.
	
	author: -W
*/
pcb_t *allocPcb() {
	if (list_empty(&pcbFree_h)) {
		return(NULL);
	}
	else {
		struct list_head *elem = list_next(&pcbFree_h);
		list_del(elem);
		pcb_PTR oggetto = container_of(elem, pcb_t, p_list);
		/* Inizializzo i campi dell'albero dei processi */
		oggetto->p_parent = NULL;
		INIT_LIST_HEAD(&(oggetto->p_child));
		INIT_LIST_HEAD(&(oggetto->p_sib)); //chiedere conferma al Tutor
		/* Inizializzo i campi riguardanti le informazioni 
		   sullo stato del processo */
		/* Campi della struct p_s di tipo state_t 
		oggetto->p_s.entry_hi = 0;
		oggetto->p_s.cause = 0;
		oggetto->p_s.status = 0;
		oggetto->p_s.pc_epc = 0;
		oggetto->p_s.gpr[STATE_GPR_LEN] = 0;
		oggetto->p_s.hi = 0;
		oggetto->p_s.lo = 0;
		Fine campi struct p_s di tipo state_t */
		oggetto->p_time = 0;
		/* Inizializzo il puntatore al semaforo bloccante */
		oggetto->p_semAdd = NULL;
		return(oggetto);
	} 
}

/*
	4. Questa funzione prende il puntatore passatogli e usa 
	la macro del kernel linux per creare una lista di PCB vuota.
*/
void mkEmptyProcQ(struct list_head * head) {
	INIT_LIST_HEAD(head);
}

/* 
	5.
	Verifica se una lista è vuota.

	head: lista da controllare

	return: 1 se la lista è vuota, 0 altrimenti

	author: -W
*/
int emptyProcQ(struct list_head *head) {
	return(list_empty(head));
}


/*  
	6. Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
	@author: Alex
*/
void insertProcQ(struct list_head *head, pcb_t *p) {
	list_add_tail(&(p->p_list), head); //se utilizzo list_add, non funziona chiedere al tutor
}

/*  
	7. Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO. Ritorna NULL se la coda non ha elementi.
	@author: Alex
*/
pcb_t *headProcQ(struct list_head *head) {
  	if (list_empty(head) ){
		return NULL;
  	}	
	return container_of(head->next, pcb_t, p_list); 
	
}

/*
	8. 
	Rimuove il primo elemento nella coda dei processi puntata da head.
	
	Return: puntatore all'elemento rimosso || NULL se la process queue è vuota.
*/
pcb_t *removeProcQ(struct list_head *head) {
	if (list_empty(head)) return NULL;
	else {
		pcb_PTR p = container_of(head->next, pcb_t, p_list);
		list_del(&(p->p_list));
		return p;
	}
}

/*
	9. 
	Rimuove il pcb puntato da p dalla coda 
	dei processi puntata da head. 
	
	head: puntatore alla testa della lista
	p: puntatore al pcb da rimuovere	

	Return: NULL se p non è presente nella coda.

	author: -W
*/
pcb_t* outProcQ(struct list_head* head, pcb_t *p) {
	pcb_PTR oggetto;
	list_for_each_entry(oggetto, head, p_list){
		if (oggetto==p) {
			list_del(&(oggetto->p_list));
			return oggetto;
		}		
	}
	return NULL;
}
/*
	10.
	Verifica se il pcb passato ha figli.

	p: puntatore a pcb da testare.
	
	Return: 1 se non ha figli, 0 altrimenti.

	author: -W
*/
int emptyChild(pcb_t *p) {
	return list_empty(&(p->p_child));
}

/*
	11. 
	Inserisce il pcb puntato da p come figlio del pcb puntato da prnt.
	
	p = puntatore a pcb da inserire 
	prnt = parent da assegnare a p

	author: -W
*/
void insertChild(pcb_t *prnt, pcb_t *p) {
	p->p_parent = prnt;
	list_add(&(p->p_sib), &(prnt->p_child)); 
}

/*
	12. 
	Rimuove il primo figlio del PCB puntato da p e lo restituisce.
	
	Return: p->p_child || NULL.
*/
pcb_t *removeChild(pcb_t *p) {
	if (emptyChild(p)) return NULL;
	else {
		struct list_head *temp = list_next(&(p->p_child));
		pcb_PTR child = container_of(temp, pcb_t, p_sib); // puntatore da ritornare
		child->p_parent = NULL;
		list_del(&(child->p_sib)); 
		return child;
	}
}
/*  chiedere al tutor quali sono i problemi di questa versione
pcb_t *removeChild(pcb_t *p) {
	if (emptyChild(p)) return NULL;
	else {
		struct list_head *temp = list_next(&(p->p_child));
		pcb_PTR child = container_of(temp, pcb_t, p_list); // chiedere al tutor perché questa non va
		child->p_parent = NULL;
		list_del(&(child->p_sib)); // chiedere al tutor perché non funziona
		return child;
	}
}
*/
/*
	13. Rimuove il PCB puntato da p dalla lista dei figli del padre.
	Se il PCB puntato da p non ha un padre, restituisce NULL, altrimenti restituisce l’elemento rimosso (cioè p).
	A differenza della removeChild, p può trovarsi in una posizione arbitraria (ossia non è necessariamente il primo figlio del padre).
	@author: Alex
*/
pcb_t *outChild(pcb_t *p) {
	if(p->p_parent == NULL) return NULL;
	struct list_head *figlidelpadre = &(p->p_parent->p_child);
	pcb_PTR temp = NULL; //conterrà uno ad uno i figli del padre
	list_for_each_entry(temp, figlidelpadre, p_sib) {
		if (p == temp) {
			list_del(&(temp->p_sib)); //modifica fatta per nuove spec scoperte su telegram
			temp->p_parent = NULL;
			return p;
		}
	}
	return NULL;
}
#include "pcb.h"

// Dichiarazione della lista dei pcb liberi
LIST_HEAD(pcbFree_h);

// Dichiarazione dell'array di pcb
pcb_t pcbFree_table[MAX_PROC];

/* 
	Viene eseguita solo all'inizio: prende tutti gli elementi della tabella pcbFree_table e li inserisce nella lista pcbFree_h
*/
void initPcbs() {
   if(list_empty(&(pcbFree_h)){
	for(int i=0; i<MAX_PROC; i++){
		list_add( &(pcbFree_table[i].p_list), &pcbFree_h );
	}
   }
}

/*
	Questa funzione prende un processo e lo riaggiunge nella lista pcbFree_h
*/
void freePcb(pcb_t *p) {
	list_add( p, &pcbFree_h );
}
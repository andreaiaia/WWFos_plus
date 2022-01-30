#include "pcb.h"

// Dichiarazione della lista dei pcb liberi
// TODO: da inizializzare con initPcb()
LIST_HEAD(pcbFree_h)

// Per maggiori info pag. 19 di pandos.pdf


// CODICE SCRITTO DA QUEL POVERACCIO DI ALEX //
pcb_t pcbFree_table[MAX_PROC];
/* Inizializza la lista pcbFree in modo da contenere tutti gli elementi della pcbFree_table. 
Questo metodo deve essere chiamato una volta sola in fase di inizializzazione della struttura dati. */
void initPcbs(){
   if(list_empty(&(pcbFree_h)){ //non sono sicuro di questo if
		for(int i=0; i<MAX_PROC; i++){
	        list_add( &(pcbFree_table[i].p_next), &(pcbFree_h) );
		}
   }  
}       

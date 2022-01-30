#include "pcb.h"

// Dichiarazione della lista dei pcb liberi
LIST_HEAD(pcbFree_h);

// Dichiarazione dell'array di pcb
pcb_t pcbFree_table[MAX_PROC];

/* 
Inizializza la lista pcbFree in modo da contenere tutti gli elementi della pcbFree_table. 
Questo metodo deve essere chiamato una volta sola in fase di inizializzazione della struttura dati. 
*/
void initPcbs(){
   if(list_empty(&(pcbFree_h)){
	for(int i=0; i<MAX_PROC; i++){
		list_add( &(pcbFree_table[i].p_next), &(pcbFree_h) );
	}
   }  
}       

#include "../h/pandos_const.h"
#include "../h/pandos_types.h"
#include "../h/listx.h"

/* Allocation and deallocation of pcbs */

/**
 * Prende tutti gli elementi della tabella pcbFree_table
 * e li inserisce nella lista pcbFree_h
 */
void initPcbs();
/**
 * Verifica che p non sia NULL e lo riaggiunge nella lista pcbFree_h.
 * @param p: puntatore al pcb da aggiungere a pcbFree_h
 */
void freePcb(pcb_t *p);
/**
 * Rimuove un processo dalla lista pcbFree_h,
 * e inizializza i suoi campi.
 * @return: pcb_t* o NULL se pcbFree_h vuota
 */
pcb_t *allocPcb();

//* Process Queue Maintenance */

/**
 * Crea una lista vuota legandola al puntatore passatogli.
 * @param head: puntatore a sentinella della lista da inizializzare
 */
void mkEmptyProcQ(struct list_head *head);
/**
 * Verifica se una lista è vuota.
 * @param head: lista da controllare
 * @return: 1 se la lista è vuota, 0 altrimenti
 */
int emptyProcQ(struct list_head *head);
/**
 * Inserisce l’elemento puntato da p nella
 * coda dei processi puntata da head.
 * @param head: sentinella della coda dei processi
 * @param p: pcb da inserire nella coda dei processi
 */
void insertProcQ(struct list_head *head, pcb_t *p);
/**
 * Restituisce (senza rimuoverlo) l’elemento di testa
 * della coda dei processi da head.
 * @param head: sentinella della coda dei processi
 * @return: pcb in testa alla coda o NULL se essa è vuota
 */
pcb_t *headProcQ(struct list_head *head);
/**
 * Rimuove il primo elemento nella coda dei
 * processi puntata da head.
 * @param head: sentinella della coda dei processi
 * @return: puntatore all'elemento rimosso o NULL se la process queue è vuota
 */
pcb_t *removeProcQ(struct list_head *head);
/**
 * Rimuove il pcb puntato da p dalla coda
 * dei processi puntata da head.
 * @param head: puntatore alla testa della lista
 * @param p: puntatore al pcb da rimuovere
 * @return: pcb rimosso dalla coda dei processi o NULL se p non è presente nella coda
 */
pcb_t *outProcQ(struct list_head *head, pcb_t *p);

//* Process Tree Maintenance */

/**
 * Verifica se il pcb passato ha figli.
 * @param p: puntatore a pcb da testare
 * @return: 1 se non ha figli, 0 altrimenti
 */
int emptyChild(pcb_t *p);
/**
 * Inserisce il pcb puntato da p come figlio
 * del pcb puntato da prnt.
 * @param prnt = parent da assegnare a p
 * @param p = puntatore a pcb da inserire
 */
void insertChild(pcb_t *prnt, pcb_t *p);
/**
 * Rimuove il primo figlio del PCB puntato da
 * p e lo restituisce.
 * @param p: puntatore al processo padre
 * @return: puntatore al processo figlio rimosso o NULL se p non ha figli
 */
pcb_t *removeChild(pcb_t *p);
/**
 * Rimuove il PCB puntato da p dalla lista dei figli del padre.
 * A differenza della removeChild, p può trovarsi in una posizione
 * arbitraria (ossia non è necessariamente il primo figlio del padre).
 * @param p: puntatore al processo da rimuovere
 * @return: restituisce l’elemento rimosso o NULL se il PCB puntato da p non ha un padre
 */
pcb_t *outChild(pcb_t *p);
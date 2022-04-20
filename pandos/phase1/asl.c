#include "asl.h"
#include "pcb.h"

// Tabella dei semafori di dimensione massima MAXPROC (allocazione in memoria dei semafori)
static semd_t semd_table[MAXPROC];
// Lista dei semafori liberi/inutilizzati
static LIST_HEAD(semdFree_h);
// Lista dei semafori attivi/utilizzati (ASL)
static LIST_HEAD(semd_h);

/**
 * Inserisce il pcb puntato da p nella coda dei
 * processi bloccati del semaforo avente chiave semAdd.
 * Se il semaforo corrispondente non è
 * presente nella ASL, ne alloca uno nuovo.
 * @param semAdd: chiave del semaforo che deve bloccare p
 * @param p: puntatore al processo da bloccare
 * @return: 1 se semdFree_h è vuota, 0 altrimenti
 */
int insertBlocked(int *semAdd, pcb_t *p)
{
    semd_PTR tmp = NULL;
    /* scorre la lista dei semafori attivi/utilizzati */
    list_for_each_entry(tmp, &semd_h, s_link)
    {
        if ((tmp->s_key == semAdd))
        {
            p->p_semAdd = semAdd;
            list_add_tail(&(p->p_list), &(tmp->s_procq)); // se aggiungiamo in testa non funziona
            return FALSE;
        }
    }
    /* Caso in cui il semaforo non è presente nella ASL */
    // return true se non ci sono semafori liberi da allocare
    if (list_empty(&semdFree_h))
        return TRUE;
    // allocazione nuovo semd dalla lista semdFree
    semd_PTR semallocato = container_of(list_next(&semdFree_h), semd_t, s_link);
    list_del(&(semallocato->s_link));
    // Abbino processo e semaforo sulla stessa chiave
    semallocato->s_key = semAdd;
    p->p_semAdd = semAdd;
    list_add(&(semallocato->s_link), &semd_h);
    list_add(&(p->p_list), &(semallocato->s_procq));
    return FALSE;
}
/**
 * Rimuove il primo PCB dalla coda dei processi bloccati
 * del semaforo con chiave semAdd.
 * Se la coda dei processi bloccati diventa vuota, rimuove il
 * semaforo dalla ASL e lo rimette nella semdFree_h.
 * @param semAdd: chiave del semaforo
 * @return: Puntatore al pcb rimosso o NULL se la coda è vuota.
 */
pcb_t *removeBlocked(int *semAdd)
{
    semd_PTR s_iter;
    pcb_PTR res = NULL;
    list_for_each_entry(s_iter, &semd_h, s_link)
    {
        if (s_iter->s_key == semAdd)
        {
            res = removeProcQ(&(s_iter->s_procq));
            if (list_empty(&(s_iter->s_procq)))
            {
                list_del(&(s_iter->s_link));
                list_add(&(s_iter->s_link), &semdFree_h);
            }
            return res;
        }
    }
    return NULL;
}
/**
 * Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato.
 * Se la coda del semaforo diventa vuota, rimuove il semaforo dalla ASL
 * e lo inserisce in semdFree_h.
 * @param p: puntatore a PCB da rimuovere.
 * @return: puntatore al PCB rimosso o NULL se non c'è p nella coda.
 */
pcb_t *outBlocked(pcb_t *p)
{
    semd_PTR sem_iteratore = NULL;
    list_for_each_entry(sem_iteratore, &semd_h, s_link)
    {
        if ((p->p_semAdd) == (sem_iteratore->s_key))
        {
            outProcQ(&(sem_iteratore->s_procq), p);
            if (list_empty(&(sem_iteratore->s_procq)))
            {
                sem_iteratore->s_key = NULL;
                list_del(&(sem_iteratore->s_link));
                list_add_tail(&(sem_iteratore->s_link), &semdFree_h);
            }
            return p;
        }
    }
    // Stato di errore
    return NULL;
}
/**
 * Restituisce (senza rimuoverlo) il puntatore al primo PCB
 * della coda dei processi del semaforo con chiave semAdd.
 * @param semAdd: chiave del semaforo.
 * @return: puntatore al primo pcb_t nella coda o NULL se la coda è vuota.
 */
pcb_t *headBlocked(int *semAdd)
{
    semd_PTR sem_iteratore;
    list_for_each_entry(sem_iteratore, &semd_h, s_link)
    {
        if ((sem_iteratore->s_key) == semAdd)
        {
            if (list_empty(&(sem_iteratore->s_procq)))
                return NULL; // La coda dei pcb è vuota
            return container_of(list_next(&(sem_iteratore->s_procq)), pcb_t, p_list);
        }
    }
    return NULL;
}
/**
 * Inizializza la lista semdFree in modo da contenere
 * tutti gli elementi della semdTable.
 */
void initASL()
{
    /**
     * Ciclo ogni elemento della tabella dei semafori e lo aggiungo
     * alla lista dei semafori liberi usando la macro del kernel linux list_add.
     */
    for (int i = 0; i < MAXPROC; i++)
    {
        list_add(&(semd_table[i].s_link), &semdFree_h);
        semd_table[i].s_key = NULL;
        INIT_LIST_HEAD(&(semd_table[i].s_procq));
    }
}
#include "asl.h"

// Tabella dei semafori di dimensione massima MAXPROC
semd_t semd_table[MAXPROC];

// Lista dei semafori liberi/inutilizzati
LIST_HEAD(semdFree_h);

// Lista dei semafori attivi/utilizzati
LIST_HEAD(semd_h);

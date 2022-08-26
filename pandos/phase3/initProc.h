#ifndef INIT_PROC_H
#define INIT_PROC_H

// Non sono sicuro del numero di semafori, dal libro
// (pagina 63) mi sembra di capire che siano 8 flash device,
// 8 stampanti e 16 terminali, quindi 32 in tutto
#define DEVSEM_NUM 32

extern void initSwapStructs();

void test_fase3();

#endif
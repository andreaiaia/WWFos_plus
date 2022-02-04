p1test: p1test.o asl.o pcb.o pandos_types.o pandos_consts.o
	gcc -o p1test p1test.o asl.o pcb.o pandos_consts.o pandos_types.o

p1test.o: phase1/p1test.c phase1/asl.h phase1/pcb.h h/pandos_consts.h h/pandos_types.h
	gcc -c phase1/p1test.c 

asl.o: phase1/asl.c phase1/asl.h h/pandos_consts.h h/pandos_types.h h/listx.h
	gcc -c phase1/asl.c

pcb.o: phase1/pcb.c phase1/pcb.h h/pandos_consts.h h/pandos_types.h h/listx.h
	gcc -c phase1/pcb.c

pandos_consts.o: h/pandos_consts.h
	gcc -c h/pandos_consts.h

pandos_types.o: h/pandos_types.h
	gcc -c h/pandos_types.h

listx.h: h/listx.h
	gcc -c h/listx.h

clean:
	rm *.o main
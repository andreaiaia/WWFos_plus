p1test: p1test.o asl.o pcb.o
	gcc -o p1test p1test.o asl.o pcb.osssssssss

asl.o: phase1/asl.c phase1/asl.h
	gcc -c phase1/asl.c

pcb.o: phase1/pcb.c phase1/pcb.h
	gcc -c phase1/pcb.c

clear:
	rm *.o p1test
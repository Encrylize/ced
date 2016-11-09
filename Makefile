interface: interface.o buffer.o line.o
	gcc -lncurses -o interface interface.o buffer.o line.o

interface.o: interface.c buffer.h line.h
	gcc -c interface.c

buffer.o: buffer.c buffer.h line.h
	gcc -c buffer.c

line.o: line.c line.h
	gcc -c line.c

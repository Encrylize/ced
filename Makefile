CC=gcc
CFLAGS=-Wall -Wextra -Wmisleading-indentation -std=c99 -ftrapv

ced: ced.o buffer.o line.o view.o alloc.o
	$(CC) -lncurses -o ced ced.o buffer.o line.o view.o alloc.o

ced.o: ced.c buffer.h line.h view.h alloc.h
	$(CC) -c ced.c

view.o: view.c view.h buffer.h alloc.h
	$(CC) -c view.c

buffer.o: buffer.c buffer.h line.h alloc.h
	$(CC) -c buffer.c

line.o: line.c line.h alloc.h
	$(CC) -c line.c

alloc.o: alloc.c alloc.h
	$(CC) -c alloc.c

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -ftrapv

interface: interface.o buffer.o line.o view.o
	$(CC) -lncurses -o interface interface.o buffer.o line.o view.o

interface.o: interface.c buffer.h line.h view.h
	$(CC) -c interface.c

view.o: view.c view.h buffer.h
	$(CC) -c view.c

buffer.o: buffer.c buffer.h line.h
	$(CC) -c buffer.c

line.o: line.c line.h
	$(CC) -c line.c

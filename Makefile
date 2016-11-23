CC=gcc
CFLAGS=-Wall -Wextra -Wmisleading-indentation -std=c99 -ftrapv

ced: ced.o buffer.o line.o view.o alloc.o editor.o
	$(CC) $(CFLAGS) -lncurses -o ced ced.o buffer.o line.o view.o alloc.o editor.o

ced.o: ced.c buffer.h line.h view.h alloc.h editor.h
	$(CC) $(CFLAGS) -c ced.c

editor.o: editor.c editor.h buffer.h view.h
	$(CC) $(CFLAGS) -c editor.c

view.o: view.c view.h buffer.h alloc.h
	$(CC) $(CFLAGS) -c view.c

buffer.o: buffer.c buffer.h line.h alloc.h
	$(CC) $(CFLAGS) -c buffer.c

line.o: line.c line.h alloc.h
	$(CC) $(CFLAGS) -c line.c

alloc.o: alloc.c alloc.h
	$(CC) $(CFLAGS) -c alloc.c

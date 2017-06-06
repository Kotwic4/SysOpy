CFLAGS += -Wall
CC = gcc

all: mainA mainB mainC generator

mainA: main.c common.h
	$(CC) $(CFLAGS) main.c -o mainA -lpthread -DASYN

mainB: main.c common.h
	$(CC) $(CFLAGS) main.c -o mainB -lpthread -DSYNC

mainC: main.c common.h
	$(CC) $(CFLAGS) main.c -o mainC -lpthread -DDETA

generator: generator.c common.h
	$(CC) $(CFLAGS) generator.c -o generator

clean:
	-rm -rf *.o mainA mainB mainC generator

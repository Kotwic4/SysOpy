CFLAGS += -Wall
CC = gcc

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main -lpthread

clean:
	-rm -rf *.o main

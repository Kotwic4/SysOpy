CFLAGS += -Wall -g
CC = gcc

all: main demo

main: main.c
	$(CC) $(CFLAGS) main.c -o main

demo: demo.c
	$(CC) $(CFLAGS) demo.c -o demo

clean:
	-rm -rf *.o main demo



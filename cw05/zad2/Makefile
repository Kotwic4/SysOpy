CFLAGS += -Wall
CC = gcc

all: slave master

slave: slave.c
	$(CC) $(CFLAGS) slave.c -o slave

master: master.c
	$(CC) $(CFLAGS) master.c -o master

clean:
	-rm -rf *.o master slave data fifo

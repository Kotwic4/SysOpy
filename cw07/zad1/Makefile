CFLAGS += -Wall
CC = gcc

all: server client factory

server: server.c common.h
	$(CC) $(CFLAGS) server.c -o server

client: client.c common.h
	$(CC) $(CFLAGS) client.c -o client

factory: factory.c
	$(CC) $(CFLAGS) factory.c -o factory


clean:
	-rm -rf *.o server client factory

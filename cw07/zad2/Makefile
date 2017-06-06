CFLAGS += -Wall
CC = gcc

all: server client factory

server: server.c common.h
	$(CC) $(CFLAGS) server.c -lpthread -lrt -o server

client: client.c common.h
	$(CC) $(CFLAGS) client.c -lpthread -lrt -o client

factory: factory.c
	$(CC) $(CFLAGS) factory.c -o factory


clean:
	-rm -rf *.o server client factory

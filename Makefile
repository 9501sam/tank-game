CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lncurses

BINS = server client
ALL: $(BINS) $(OBJS)

OBJS = server.o client.o net.o ui.o game.o

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS) $(LDFLAGS)

server: server.o net.o ui.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

client: client.o net.o ui.o game.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

ser: server
	./server

cli: client
	./client

clean:
	$(RM) $(BINS) $(OBJS)

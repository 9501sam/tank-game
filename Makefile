CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lncurses -pthread

BINS = server client
ALL: $(BINS) $(OBJS)

OBJS = server.o client.o network.o ui.o game.o map.o tank.o bullet.o

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS) $(LDFLAGS)

server: server.o # network.o ui.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

client: client.o network.o ui.o game.o map.o tank.o bullet.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

ser: server
	./server

cli: client
	./client

clean:
	$(RM) $(BINS) $(OBJS)

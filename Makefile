CC = gcc
CFLAGS = -Wall -O3 -g
LDFLAGS = -lncurses -pthread

BINS = server client
ALL: $(BINS) $(OBJS)

OBJS = server.o client.o network.o ui.o game.o tank.o bullet.o

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

server: server.o network.o
	$(CC) -o $@ $^ $(CFLAGS)

client: client.o network.o ui.o game.o tank.o bullet.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

ser: server
	./server 1234

cli: client
	./client 127.0.0.1 1234

clean:
	$(RM) $(BINS) $(OBJS)

image:
	docker build -t tank-game/server .

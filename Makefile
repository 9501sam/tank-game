CC = gcc
CFLAGS = -Wall -O3 -g

NCSLDFLAGS = -lncurses -pthread
OGLLDFLAGS = -lglfw -lGL -lm -lX11 -lpthread -lXi -lXrandr -ldl
ifeq ("$(UI)", "OPENGL")
	LDFLAGS = $(OGLLDFLAGS)
else
	LDFLAGS = $(NCSLDFLAGS)
endif

BINS = server client

ifeq ("$(UI)", "OPENGL")
	UIOBJ = gui.o
else
	UIOBJ = ui.o
endif

ALL: $(BINS) $(OBJS)

OBJS = server.o client.o network.o $(UIOBJ) game.o tank.o bullet.o glad.o

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

server: server.o network.o
	$(CC) -o $@ $^ $(CFLAGS)

client: client.o network.o $(UIOBJ) game.o tank.o bullet.o glad.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

ser: server
	./server 1234

cli: client
	./client 127.0.0.1 1234

clean:
	$(RM) $(BINS) $(OBJS) ui.o gui.o

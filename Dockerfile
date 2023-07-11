FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    gcc \
    make

WORKDIR /tank-game

COPY Makefile .
COPY tankio.h .
COPY network.c .
COPY server.c .

RUN apt-get install make gcc libncurses5-dev -y
RUN make server

CMD ["./server", "1234"]

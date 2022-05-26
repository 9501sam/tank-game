#include "tankio.h"

static int connect_to_serv(char *serv_addr, int port)
{
    int sockfd;
    int nbytes;
    struct sockaddr_in server;
    struct packet pkt;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        err_exit("error: connect_to_serv\n");

    server.sin_addr.s_addr = inet_addr(serv_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if ((connect(sockfd, (struct sockaddr *)&server, sizeof(server))) < 0)
        err_exit("err: connect_to_serv\n");
    // get new tank for my_tank
    if ((nbytes = recv_packet(sockfd, &pkt)) == -1)
        err_exit("connect_to_serv\n");
    if (nbytes == 0)
        err_exit("game room is full\n");
    if (pkt.kind != NEW_TANK)
        err_exit("connect_to_serv\n");
    my_tank = pkt.data.tk;
    printf("%d\n", sockfd);
    return sockfd;
}

void deinit_client(void)
{
    deinit_ui();
    close(client_sock);
}

int main(int argc, char **argv)
{
    char *serv_addr;
    int port;
    if (argc != 3)
        err_exit("Usage: ./client <IP address> <port>.\n");

    serv_addr = argv[1];
    port = atoi(argv[2]);

    if (atexit(deinit_client))
        err_exit("atexit");

    if ((client_sock = connect_to_serv(serv_addr, port)) < 0)
        err_exit("fail to connect to server\n");

    start_game();

    exit(EXIT_SUCCESS);
}

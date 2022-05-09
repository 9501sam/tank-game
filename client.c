#include <stdlib.h>
#include <unistd.h>

#include "tankio.h"

int connect_to_serv(char *serv_addr, int port)
{
    int sockfd;
    struct sockaddr_in server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        err_exit("error: connect_to_serv\n");

    server.sin_addr.s_addr = inet_addr(serv_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
        perror("connect_to_serv\n");

    return sockfd;
}

int main(int argc, char **argv)
{
    char *serv_addr;
    int game_sockfd;
    int port;
    if (argc != 3)
        err_exit("Usage: ./client <IP address> <port>.\n");

    serv_addr = argv[1];
    port = atoi(argv[2]);

    game_sockfd = connect_to_serv(serv_addr, port);

    if (atexit(deinit_ui))
        err_exit("atexit");

    start_game(game_sockfd);

    close(game_sockfd);
    exit(EXIT_SUCCESS);
}

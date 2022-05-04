#include "network.h"
#include "tankgame.h"

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

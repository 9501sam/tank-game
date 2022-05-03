#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "tankgame.h"

int main(int argc, char **argv)
{
    int sockfd, port, cli01_sockfd, cli02_sockfd;
    struct sockaddr_in serv_addr, cli01_addr, cli02_addr;
    socklen_t clilen01 = sizeof(cli01_addr);
    socklen_t clilen02 = sizeof(cli02_addr);

    if (argc != 2)
        err_exit("Usage: ./server <port>.\n");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        err_exit("error: server\n");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    port = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        err_exit("error: server bind()\n");
    listen(sockfd, 5);

    cli01_sockfd = accept(sockfd, (struct sockaddr *) &cli01_sockfd, &clilen01);
    cli02_sockfd = accept(sockfd, (struct sockaddr *) &cli02_sockfd, &clilen02);

    close(cli01_sockfd);
    close(cli02_sockfd);
    close(sockfd);
}

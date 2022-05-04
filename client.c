#include <stdlib.h>
#include <unistd.h>

#include "ui.h"
#include "game.h"
#include "network.h"

int main(int argc, char **argv)
{
    char *serv_addr;
    int port;
    if (argc != 3)
        err_exit("Usage: ./client <IP address> <port>.\n");

    serv_addr = argv[1];
    port = atoi(argv[2]);

    sockfd = connect_to_serv(serv_addr, port);

    if (atexit(deinit_ui))
        err_exit("atexit");

    init_ui();

    start_game();

    close(sockfd);
    exit(EXIT_SUCCESS);
}

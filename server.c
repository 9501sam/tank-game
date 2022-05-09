#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "tankio.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int main(int argc, char **argv)
{
    if (argc < 2)
        err_exit("Usage: server <Port>\n");

    fd_set master;
    fd_set read_fds;
    int fdmax;

    int listenefd;
    int newfd;
    int port = atoi(argv[1]);
    struct sockaddr_in servaddr;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    int nbytes;
    struct action act[1];

    listenefd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenefd < 0)
        err_exit("error: server listenefd\n");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    bind(listenefd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(listenefd, 5);

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(listenefd, &master);
    fdmax = listenefd;
    
    while (1) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) // block here
            err_exit("error: select()\n");
        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listenefd) {    /* handle new connections */
                    addrlen = sizeof(remoteaddr);
                    newfd = accept(listenefd, (struct sockaddr *)&remoteaddr,
                            &addrlen);
                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master);
                        fdmax = MAX(fdmax, newfd);
                    }
                    printf("selectserver: new connection on "
                            "socket %d\n", newfd);
                } else {                /* handle data from a client */
                    if ((nbytes = recv(i, &act, sizeof(act), 0)) <= 0) {
                        if (nbytes == 0) // connection closed
                            printf("server: socket %d hung up\n", i);
                        else
                            perror("recv");
                        close(i); // bye!
                        FD_CLR(i, &master);
                    } else {            // got data from client
                        printf("got data from client\n");
                        printf("%d\n", act->data);
                        for (int j = 0; j <= fdmax; j++) {
                            if ((FD_ISSET(j, &master)) && 
                                    (j != listenefd) && (j != i)) {
                                if (send(j, (void *)act, nbytes, 0) == -1)
                                    perror("send");
                                // printf("%d %d\n", act->tk.x, act->tk.y);
                            }
                        }
                    }
                } 
            } 
        }
    }

    exit(EXIT_SUCCESS);
}

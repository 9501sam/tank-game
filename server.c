#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "tankio.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int id_to_fd[MAX_USERS];
int fd_to_id[MAX_FD];

tank tanks[MAX_USERS];

fd_set master;
fd_set read_fds;
int fdmax;

int listenefd;
int newfd;
int port;
struct sockaddr_in servaddr;
struct sockaddr_storage remoteaddr;
socklen_t addrlen;

int nbytes;
struct package pkg;

static void assign_tank_id(const int newfd)
{
    for (int i = 0; i < MAX_USERS; i++)
        if (id_to_fd[i] == -1) {
            if ((send(newfd, &i, sizeof(i), 0)) < 0)
                perror("error: assign_tank_id\n");
            id_to_fd[i] = newfd;
            fd_to_id[newfd] = i;
            return;
        }
}

static void release_tank_id(const int fd)
{
    int id = fd_to_id[fd];
    id_to_fd[id] = -1;
    fd_to_id[fd] = -1;
}

static void handle_new_connect()
{
    addrlen = sizeof(remoteaddr);
    newfd = accept(listenefd, (struct sockaddr *)&remoteaddr,
            &addrlen);
    if (newfd == -1) {
        perror("accept");
    } else {
        FD_SET(newfd, &master);
        fdmax = MAX(fdmax, newfd);
    }
    assign_tank_id(newfd);
    printf("selectserver: new connection on "
            "socket %d\n", newfd);
}

static void handle_data(int fd)
{
    if ((nbytes = recv(fd, &pkg, sizeof(pkg), 0)) <= 0) {
        if (nbytes == 0) {                                  // 1. connection closed
            printf("server: socket %d hung up\n", fd);
        } else {                                            // 2. error on got data
            perror("recv");
            struct package tmp = {.kind = DIE, .data.die_id = fd_to_id[fd]};
            for (int j = 0; j <= fdmax; j++) {
                if ((FD_ISSET(j, &master)) && (j != listenefd) && (j != fd))
                    if (send(j, &tmp, nbytes, 0) == -1)
                        perror("send");
            }
        }
        ///*** player fd is die, bye!!! ***///
        close(fd);
        FD_CLR(fd, &master);
        release_tank_id(fd);
    } else {                                                // 3. success got data from client
                                                            // NEW_TANK, TANK, BULLET, ATTACKED or DIE
        if (pkg.kind == DIE) {              // DIE
            release_tank_id(fd);
        } else if (pkg.kind == NEW_TANK) {  // NEW_TANK
            for (int i = 0; i <= fdmax; i++) {
                if ((FD_ISSET(i, &master)) && (i != listenefd) && (i != fd)) {
                    int id = fd_to_id[i];
                    struct package pkg = {
                        .kind = TANK,
                        .data.tk = tanks[id],
                    };
                    if ((send(fd, &pkg, sizeof(pkg), 0)) == -1)
                        perror("handle_data\n");
                }
            }
        } else if (pkg.kind == TANK) {      // TANK
            int id = pkg.data.tk.id;
            tanks[id] = pkg.data.tk;
        }

        for (int j = 0; j <= fdmax; j++) {
            if ((FD_ISSET(j, &master)) && (j != listenefd) && (j != fd))
                if (send(j, &pkg, nbytes, 0) == -1)
                    perror("send");
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
        err_exit("Usage: server <Port>\n");

    port = atoi(argv[1]);

    memset(id_to_fd, -1, sizeof(id_to_fd));
    memset(fd_to_id, -1, sizeof(fd_to_id));

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
            if (FD_ISSET(i, &read_fds)) { ///*** we got one ***///
                if (i == listenefd) {    /* handle new connections */
                    handle_new_connect();
                } else {                /* handle data from a client */
                    handle_data(i);
                } 
            } 
        }
    }

    exit(EXIT_SUCCESS);
}

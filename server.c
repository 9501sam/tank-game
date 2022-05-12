#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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

void close_all_fd(void)
{
    for (int i = 0; i <= fdmax; i++)
        if (FD_ISSET(i, &master))
            close(i);
    return NULL;
}

static void assign_tank_id(const int newfd)
{
    for (int i = 0; i < MAX_USERS; i++)
        if (id_to_fd[i] == NOT_USED) {
            id_to_fd[i] = newfd;
            fd_to_id[newfd] = i;
            return;
        }
}

static void release_tank_id(const int fd)
{
    int id = fd_to_id[fd];
    id_to_fd[id] = NOT_USED;
    fd_to_id[fd] = NOT_USED;
}

static void handle_new_connect()
{
    int newid;
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
    newid = fd_to_id[newfd];
    printf("selectserver: new connection:"
            "fd=%d, id=%d\n"
            , newfd, newid);

    tank newtk = {
        .x = 10,
        .y = 10,
        .dir = UP,
        .ph = DEFAULT_PH,
        .id = newid,
    };
    struct package newtk_pkg = {
        .kind = NEW_TANK,
        .data.tk = newtk,
    };
    tanks[newid] = newtk;
    // talk to new player about new tank and 
    // its enemies
    if (send(newfd, &newtk_pkg, sizeof(newtk_pkg), 0) == -1)
        perror("handle_data(): send()");
    // printf("NEW_TANK, fd=%d x=%d y=%d\n", newfd, tk.x, tk.y);
    for (int i = 0; i <= fdmax; i++) {
        if ((FD_ISSET(i, &master)) && (i != listenefd) && (i != newfd)) {
            int id = fd_to_id[i];
            tank tk = tanks[id];
            struct package pkg = {
                .kind = NEW_TANK,
                .data.tk = tk,
            }; 
            if (send(newfd, &pkg, sizeof(pkg), 0) == -1)
                perror("send");
            // printf("NEW_TANK, fd=%d x=%d y=%d\n", newfd, tk.x, tk.y);
        }
    }

    // talk to all the other players about the 
    // new player
    for (int i = 0; i <= fdmax; i++) {
        if ((FD_ISSET(i, &master)) && (i != listenefd) && (i != newfd))
            if (send(i, &newtk_pkg, sizeof(newtk_pkg), 0) == -1)
                perror("send");
    }
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
                    if (send(j, &tmp, sizeof(tmp), 0) == -1)
                        perror("send");
            }
        }
        ///*** player fd is die, bye!!! ***///
        close(fd);
        FD_CLR(fd, &master);
        release_tank_id(fd);
    } else {                                                // 3. success got data from client
                                                            // TANK, BULLET, ATTACKED or DIE
        if (pkg.kind == DIE) {              // DIE
            release_tank_id(fd);
        } else if (pkg.kind == TANK) {      // TANK
            int id = pkg.data.tk.id;
            tanks[id] = pkg.data.tk;
        }

        for (int j = 0; j <= fdmax; j++) {
            if ((FD_ISSET(j, &master)) && (j != listenefd) && (j != fd))
                if (send(j, &pkg, sizeof(pkg), 0) == -1)
                    perror("send");
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
        err_exit("Usage: server <Port>\n");

    port = atoi(argv[1]);

    memset(id_to_fd, NOT_USED, sizeof(id_to_fd));
    memset(fd_to_id, NOT_USED, sizeof(fd_to_id));

    if (atexit(close_all_fd))
        err_exit("atexit");

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

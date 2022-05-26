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
int num_users = 0;

void close_all_fd(void)
{
    for (int i = 0; i <= fdmax; i++)
        if (FD_ISSET(i, &master))
            close(i);
}

static void player_die(int fd)
{
    int id = fd_to_id[fd];
    struct packet dietk_pkt = {.kind = DIE, .data.id = id};
    for (int j = 0; j <= fdmax; j++) {
        if ((FD_ISSET(j, &master)) && (j != listenefd) && (j != fd))
            if (send_packet(j, &dietk_pkt) == -1)
                perror("send");
    }
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

static void handle_new_connect(void)
{
    int newid;
    addrlen = sizeof(remoteaddr);
    newfd = accept(listenefd, (struct sockaddr *)&remoteaddr,
            &addrlen);
    if (newfd == -1) {
        perror("accept");
    } else if (num_users + 1 > MAX_USERS) {
        close(newfd);
        return;
    } else {
        num_users++;
        FD_SET(newfd, &master);
        fdmax = MAX(fdmax, newfd);
    }

    assign_tank_id(newfd);
    newid = fd_to_id[newfd];
    printf("selectserver: new connection:"
            "fd=%d, id=%d\n",
              newfd, newid);

    tank newtk = {
        .x = 20,
        .y = 15,
        .dir = UP,
        .hp = DEFAULT_HP,
        .nblts = NUM_BULLETS,
        .id = newid,
    };
    struct packet newtk_pkg = {
        .kind = NEW_TANK,
        .data.tk = newtk,
    };
    tanks[newid] = newtk;
    // talk to new player about new tank and 
    // its enemies
    if (send_packet(newfd, &newtk_pkg) == -1)
        perror("handle_data(): send()");
    for (int i = 0; i <= fdmax; i++) {
        if ((FD_ISSET(i, &master)) && (i != listenefd) && (i != newfd)) {
            int id = fd_to_id[i];
            tank tk = tanks[id];
            struct packet pkg = {
                .kind = NEW_TANK,
                .data.tk = tk,
            }; 
            if (send_packet(newfd, &pkg) == -1)
                perror("send");
        }
    }
    // talk to all the other players about the 
    // new player
    for (int i = 0; i <= fdmax; i++) {
        if ((FD_ISSET(i, &master)) && (i != listenefd) && (i != newfd))
            if (send_packet(i, &newtk_pkg) == -1)
                perror("send");
    }
}

static void handle_data(int fd)
{
    struct packet pkg;
    if ((nbytes = recv_packet(fd, &pkg)) <= 0) {
        if (nbytes == 0) {                                  // 1. connection closed
            printf("server: socket %d hung up\n", fd);
            player_die(fd);
        } else {                                            // 2. error on got data
            perror("recv");
            player_die(fd);
        }
        // player fd is die, bye!!!
        close(fd);
        FD_CLR(fd, &master);
        release_tank_id(fd);
    } else {                                                // 3. success got data from client
                                                            // NEW_TANK, TANK, SHOOT, ATTACKED or DIE
        if (pkg.kind == DIE) {              // DIE
            release_tank_id(fd);
        } else if (pkg.kind == TANK) {      // TANK
            int id = pkg.data.tk.id;
            tanks[id] = pkg.data.tk;
        }

        for (int j = 0; j <= fdmax; j++) {
            if ((FD_ISSET(j, &master)) && (j != listenefd) && (j != fd))
                if (send_packet(j, &pkg) == -1)
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

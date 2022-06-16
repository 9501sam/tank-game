#include "tankio.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define _ BLOCK_EMPTY
#define W BLOCK_WALL

int id_to_fd[MAX_USERS];
int fd_to_id[MAX_FD];

tank tanks[MAX_USERS];

fd_set master;
fd_set read_fds;
int fdmax;

int listenfd;
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
        if ((FD_ISSET(j, &master)) && (j != listenfd) && (j != fd))
            if (send_packet(j, &dietk_pkt) == -1)
                perror("send");
    }
    num_users--;
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

static void get_empty_yx(int *y, int *x)
{
    int map[MAP_HEIGHT][MAP_WIDTH] = INIT_MAP;
    for (int id = 0; id < MAX_USERS; id++)
        if (id_to_fd[id] != NOT_USED) {
            int r = tanks[id].y;
            int c = tanks[id].x;
            for (int i = -1; i <= 1; i++)
                for (int j = -1; j <= 1; j++)
                    map[r + i][c + j] = id;
        }
    while (1) {
        *y = rand() % (MAP_HEIGHT - 4) + 2;
        *x = rand() % (MAP_WIDTH - 4) + 2;
        bool is_empty = true;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++)
                if (map[*y + i][*x + j] != BLOCK_EMPTY) {
                    is_empty = false;
                    break;
                }
            if (!is_empty)
                break;
        }
        if (is_empty)
            break;
    }
}

static void handle_new_connect(void)
{
    int newid;
    addrlen = sizeof(remoteaddr);
    newfd = accept(listenfd, (struct sockaddr *)&remoteaddr,
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

    int y, x;
    get_empty_yx(&y, &x);
    assign_tank_id(newfd);
    newid = fd_to_id[newfd];
    printf("selectserver: new connection:"
            "fd=%d, id=%d\n",
            newfd, newid);

    tank newtk = {
        .x = x,
        .y = y,
        .dir = UP,
        .hp = DEFAULT_HP,
        .nblts = NUM_BULLETS,
        .id = newid,
    };
    struct packet newtk_pkt = {
        .kind = NEW_TANK,
        .data.tk = newtk,
    };
    tanks[newid] = newtk;
    // talk to new player about new tank and 
    // its enemies
    if (send_packet(newfd, &newtk_pkt) == -1)
        perror("handle_data(): send()");
    for (int i = 0; i <= fdmax; i++) {
        if ((FD_ISSET(i, &master)) && (i != listenfd) && (i != newfd)) {
            int id = fd_to_id[i];
            tank tk = tanks[id];
            struct packet pkt = {
                .kind = NEW_TANK,
                .data.tk = tk,
            }; 
            if (send_packet(newfd, &pkt) == -1)
                perror("send");
        }
    }
    // talk to all the other players about the 
    // new player
    for (int i = 0; i <= fdmax; i++) {
        if ((FD_ISSET(i, &master)) && (i != listenfd) && (i != newfd))
            if (send_packet(i, &newtk_pkt) == -1)
                perror("send");
    }
}

static void handle_data(int fd)
{
    struct packet pkt;
    if ((nbytes = recv_packet(fd, &pkt)) <= 0) {
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
                                                            // TANK, SHOOT, ATTACKED or DIE
        int id;
        switch (pkt.kind) {
        case NEW_TANK:
            perror("NEW_TANK should not be here.\n");
            break;
        case TANK:
            id = pkt.data.tk.id;
            tanks[id] = pkt.data.tk;
            break;
        case SHOOT:
            id = pkt.data.id;
            tanks[id].nblts--;
            break;
        case REFILL:
            id = pkt.data.id;
            tanks[id].nblts = NUM_BULLETS;
            break;
        case ATTACKED:
            id = pkt.data.id;
            tanks[id].hp--;
            break;
        case DIE:
            release_tank_id(fd);
            break;
        default:
            break;
        }

        for (int j = 0; j <= fdmax; j++) {
            if ((FD_ISSET(j, &master)) && (j != listenfd) && (j != fd))
                if (send_packet(j, &pkt) == -1)
                    perror("send");
        }
    }
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv)
{
    if (argc < 2)
        err_exit("Usage: server <Port>\n");

    char *portstr = argv[1];
    port = atoi(portstr);

    memset(id_to_fd, NOT_USED, sizeof(id_to_fd));
    memset(fd_to_id, NOT_USED, sizeof(fd_to_id));

    if (atexit(close_all_fd))
        err_exit("atexit");

    srand(time(NULL));

    int yes;
    int rv;
    struct addrinfo hints, *ai, *p;

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, portstr, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listenfd < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) < 0) {
            close(listenfd);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listenfd, 10) == -1) {
        perror("listen");
        exit(3);
    }

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(listenfd, &master);
    fdmax = listenfd;

    while (1) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) // block here
            err_exit("error: select()\n");
        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { ///*** we got one ***///
                if (i == listenfd) {    /* handle new connections */
                    handle_new_connect();
                } else {                /* handle data from a client */
                    handle_data(i);
                } 
            } 
        }
    }

    exit(EXIT_SUCCESS);
}

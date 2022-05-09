#include <pthread.h>
#include <string.h>

#include "tankio.h"

/* these global variable should be syc using lock */
tank             enemies[MAX_USERS];
tank             my_tank;
int              client_sock;
pthread_mutex_t  lock;

static void init_game(void)
{
    // tank
    int id;
    if ((recv(client_sock, &id, sizeof(id), 0)) == -1)
        err_exit("game: init_game\n");

    my_tank.x = 10;
    my_tank.y = 10;
    my_tank.dir = UP;
    my_tank.id = id;
    my_tank.ph = DEFAULT_PH;

    struct package pkg = {.kind = NEW_TANK, .data.newtk = my_tank};
    if ((send(client_sock, &pkg, sizeof(id), 0)) == -1)
        err_exit("game: init_game, send NEW_TANK\n");

    // enemies
    for (int i = 0; i < MAX_USERS; i++)
        enemies[i].id = -1;
}

static void main_loop(void)
{
    input_t in;
    bool is_change = false;
    while (1) {
        in = get_input();
        if (in == INPUT_INVALID)
            continue;

        pthread_mutex_lock(&lock);

        switch (in) {
        case INPUT_LEFT:
            is_change = turn(&my_tank, LEFT);
            is_change = goforward(&my_tank) || is_change;
            break;
        case INPUT_RIGHT:
            is_change = turn(&my_tank, RIGHT);
            is_change = goforward(&my_tank) || is_change;
            break;
        case INPUT_UP:
            is_change = turn(&my_tank, UP);
            is_change = goforward(&my_tank) || is_change;
            break;
        case INPUT_DOWN:
            is_change = turn(&my_tank, DOWN);
            is_change = goforward(&my_tank) || is_change;
            break;
        case INPUT_FIRE:
            break;
        case INPUT_QUIT:
            return;
            break;
        default:
            is_change = false;
        }
        pthread_mutex_unlock(&lock);
    }
}

bool add_enemy(tank *tk)
{
    if (enemies[tk->id].id != -1)
        return false;
    enemies[tk->id] = *tk;
    return true;
}

bool del_enemy(int id)
{
    if (enemies[id].id == -1)
        return false;
    enemies[id].id = -1;
    return true;
}

void start_game(int fd)
{
    pthread_t t1;

    client_sock = fd;
    init_game();
    init_ui();

    pthread_create(&t1, NULL, recv_thread, NULL);
    main_loop();
    pthread_join(t1, NULL);
}

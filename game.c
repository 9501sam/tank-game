#include <pthread.h>

#include "tankio.h"

/* these global variable should be syc using lock */
tank             enemies[MAX_USERS];
tank             my_tank;
int              client_sock;
pthread_mutex_t  lock;

static void init_game(void)
{
    // tank
    my_tank.x = 10;
    my_tank.y = 10;
    my_tank.dir = UP;
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

void add_enemy(tank *tk)
{
}

void del_enemy(tank *tk)
{
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

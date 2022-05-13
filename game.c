#include <pthread.h>
#include <string.h>

#include "tankio.h"

/* these global variables should be sync using lock */
tank             enemies[MAX_USERS];
tank             my_tank;
int              client_sock;
int              map[MAP_HEIGHT][MAP_WIDTH];
pthread_mutex_t  lock;

static void init_game(void)
{
    memset(map, BLOCK_EMPTY, sizeof(map));
    for (int i = 0; i < MAX_USERS; i++)
        enemies[i].id = NOT_USED;
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
        attron_mytk();
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
        attroff_mytk();
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

void start_game(void)
{
    pthread_t t1;

    init_game();
    init_ui();

    pthread_create(&t1, NULL, recv_thread, NULL);
    main_loop();
    pthread_join(t1, NULL);
}

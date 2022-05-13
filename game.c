#include "tankio.h"

/* these global variables should be sync using lock */
tank             enemies[MAX_USERS];
tank             my_tank;
int              client_sock;
int              map[MAP_HEIGHT][MAP_WIDTH];
pthread_mutex_t  lock = PTHREAD_MUTEX_INITIALIZER;

static void init_game(void)
{
    memset(map, BLOCK_EMPTY, sizeof(map));
    for (int i = 0; i < MAX_USERS; i++)
        enemies[i].id = NOT_USED;
}

static void main_loop(void)
{
    input_t in;
    while (1) {
        in = get_input();
        if (in == INPUT_INVALID)
            continue;
        pthread_mutex_lock(&lock);
        switch (in) {
        case INPUT_LEFT:
            attron_mytk();
            turn(&my_tank, LEFT);
            goforward(&my_tank);
            attroff_mytk();
            break;
        case INPUT_RIGHT:
            attron_mytk();
            turn(&my_tank, RIGHT);
            goforward(&my_tank);
            attroff_mytk();
            break;
        case INPUT_UP:
            attron_mytk();
            turn(&my_tank, UP);
            goforward(&my_tank);
            attroff_mytk();
            break;
        case INPUT_DOWN:
            attron_mytk();
            turn(&my_tank, DOWN);
            goforward(&my_tank);
            attroff_mytk();
            break;
        case INPUT_FIRE:
            fire_thread_create(&my_tank);
            break;
        case INPUT_QUIT:
            return;
            break;
        case INPUT_INVALID:
            break;
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

void start_game(void)
{
    pthread_t t1;

    init_game();
    init_ui();
    pthread_create(&t1, NULL, recv_thread, NULL);
    main_loop();
    pthread_join(t1, NULL);
}

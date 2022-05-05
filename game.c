#include <pthread.h>

#include "game.h"
#include "ui.h"
#include "tankgame.h"

// tank     *tanks[MAX_USERS];
tank    my_tank;
bullet  bullets[MAX_BULLETS];
int     sockfd;

struct action act[1];

static void init_game(void)
{
    // tank
    my_tank.x = 10;
    my_tank.y = 10;
    my_tank.dir = UP;

    // bullets
    bullets[0].x = 0;
}

static void main_loop(struct thread_data *data)
{
    input_t in;
    bool is_change = false;
    while (1) {
        in = get_input();
        if (in == INPUT_INVALID)
            continue;

        pthread_mutex_lock(&data->lock);

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
            pthread_create(&data->thread_id, NULL, fire, (void *) data);
            break;
        case INPUT_QUIT:
            return;
            break;
        default:
            is_change = false;
        }
        if (is_change) {
            refresh_screen();
            send(sockfd, &act, sizeof(act), 0);
        }
        pthread_mutex_unlock(&data->lock);
    }
}

void start_game(int fd)
{
    init_game();

    refresh_screen();

    struct thread_data data = {.lock = PTHREAD_MUTEX_INITIALIZER,
        .thread_id = 0};

    // pthread_create(&data.thread_id, NULL, fire, (void *) &data);

    main_loop(&data);
    pthread_join(data.thread_id, NULL);
}

#include "game.h"
#include "ui.h"
#include "macros.h"

#include <ncurses.h>
#include <pthread.h>

map      *game_map;
tank     *tanks[MAX_USERS];

tank my_tank;

static void init_game(void)
{
    game_map = newmap();
    if (game_map == NULL)
        err_exit("init_game");

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
        if (is_change)
            refresh_screen();
        pthread_mutex_unlock(&data->lock);
    }
}

void start_game(void)
{
    init_game();

    refresh_screen();

    struct thread_data data = {.lock = PTHREAD_MUTEX_INITIALIZER,
        .thread_id = 0};

    // pthread_create(&data.thread_id, NULL, fire, (void *) &data);

    main_loop(&data);
    pthread_join(data.thread_id, NULL);
}

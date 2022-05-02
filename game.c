#include "game.h"
#include "ui.h"

#include <ncurses.h>

map      *game_map;
tank     *tanks[MAX_USERS];
bullet   *bullets[MAX_BULLETS];

tank my_tank = {
    .x = 10,
    .y = 10,
    .dir = UP,
};

static void init_game(void)
{
    game_map = newmap();
    if (game_map == NULL)
        err_exit("init_game");
}

static void main_loop()
{
    int c;
    while (1) {
        c = get_input();
        switch (c) {
        case KEY_LEFT:
            turn(&my_tank, LEFT);
            goforward(&my_tank);
            break;

        case KEY_RIGHT:
            turn(&my_tank, RIGHT);
            goforward(&my_tank);
            break;

        case KEY_UP:
            turn(&my_tank, UP);
            goforward(&my_tank);
            break;

        case KEY_DOWN:
            turn(&my_tank, DOWN);
            goforward(&my_tank);
            break;

        case KEY_DOWN:
            turn(&my_tank, DOWN);
            goforward(&my_tank);
            break;
        }
        refresh_screen();
    }
}

void start_game(void)
{
    init_game();
    main_loop();
}

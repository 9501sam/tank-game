#include <ncurses.h>

#include "ui.h"
#include "game.h"
#include "bullet.h"

#define PRINT_BLOCK(window, pos_y, pos_x)                         \
    do {                                                          \
        mvwaddch((window), (pos_y), ((pos_x) << 1), ACS_CKBOARD); \
        waddch((window), ACS_CKBOARD);                            \
    } while (0)

WINDOW *win_game;

const int tank_pattern[NUM_DIR][TANK_SIZE][TANK_SIZE] = {
    [LEFT] = {
        {0, 1, 1},
        {1, 1, 1},
        {0, 1, 1},
    },
    [RIGHT] = {
        {1, 1, 0},
        {1, 1, 1},
        {1, 1, 0},
    },
    [UP] = {
        {0, 1, 0},
        {1, 1, 1},
        {1, 1, 1},
    },
    [DOWN] = {
        {1, 1, 1},
        {1, 1, 1},
        {0, 1, 0},
    },
};
    
static void draw_my_tank(void)
{
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
            if (tank_pattern[my_tank.dir][i + 1][j + 1])
                PRINT_BLOCK(win_game, my_tank.y + i, my_tank.x + j);
}

static void draw_bullets(void)
{
    PRINT_BLOCK(win_game, bullets[0].y, bullets[0].x);
}

void init_ui()
{
    initscr();
    curs_set(0);
    noecho();

    int starty, startx, height, width;
    height = MAP_HEIGHT;
    width = MAP_WIDTH * 2;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;
    win_game = newwin(height, width, starty, startx);
    box(win_game, 0, 0);
    draw_my_tank();
    refresh();
    wrefresh(win_game);

    // nodelay(win_game, TRUE);
    keypad(stdscr, TRUE);
    keypad(win_game, TRUE);
}

void deinit_ui(void)
{
    endwin();
}

void refresh_screen(void)
{
    clear();
    mvprintw(0, 0, "tank:(%d,%d) bullet: (%d,%d)", 
            my_tank.x, my_tank.y, 
            bullets[0].x, bullets[0].y);
    refresh();

    wclear(win_game);

    draw_my_tank();
    draw_bullets();

    box(win_game, 0, 0);
    wrefresh(win_game);
}

input_t get_input(void)
{
    int c = wgetch(win_game);
    switch (c) {
    case KEY_LEFT:
        return INPUT_LEFT;
        break;
    case KEY_RIGHT:
        return INPUT_RIGHT;
        break;
    case KEY_UP:
        return INPUT_UP;
        break;
    case KEY_DOWN:
        return INPUT_DOWN;
        break;
    case ' ':
        return INPUT_FIRE;
        break;
    case 'q':
    case 'Q':
        return INPUT_QUIT;
        break;
    }
    return INPUT_INVALID;
}

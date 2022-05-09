#include <ncurses.h>

#include "tankio.h"

#define PRINT_BLOCK(window, pos_y, pos_x)                         \
    do {                                                          \
        mvwaddch((window), (pos_y), ((pos_x) << 1), ACS_CKBOARD); \
        waddch((window), ACS_CKBOARD);                            \
    } while (0)

#define ERASE_BLOCK(window, pos_y, pos_x)                 \
    do {                                                  \
        mvwaddch((window), (pos_y), ((pos_x) << 1), ' '); \
        waddch((window), ' ');                            \
    } while (0)

WINDOW *win_game;

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
    print_tank(&my_tank);
    refresh();
    wrefresh(win_game);

    // nodelay(win_game, TRUE);
    keypad(stdscr, TRUE);
    keypad(win_game, TRUE);
}

void deinit_ui(void)
{
    endwin();
    close(client_sock);
}


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
    
void print_tank(const tank *tk)
{
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
            if (tank_pattern[tk->dir][i + 1][j + 1])
                PRINT_BLOCK(win_game, tk->y + i, tk->x + j);
}

void erase_tank(const tank *tk)
{
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
            if (tank_pattern[tk->dir][i + 1][j + 1])
                ERASE_BLOCK(win_game, tk->y + i, tk->x + j);
}

void print_bullet(const bullet *blt)
{
    PRINT_BLOCK(win_game, blt->y, blt->x);
}

void erase_bullet(const bullet *blt)
{
    ERASE_BLOCK(win_game, blt->y, blt->x);
}

void refresh_screen(void)
{
    // clear();
    // mvprintw(0, 0, "tank:(%d,%d)", 
    //         my_tank.x, my_tank.y);
    // refresh();

    // wclear(win_game);
    // print_tank(&my_tank);
    // box(win_game, 0, 0);
    // wrefresh(win_game);
    refresh();
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

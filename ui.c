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

void attron_mytk(void)
{
    init_pair(PAIR_MYTK, COLOR_MYTK, COLOR_BLACK);
    wattron(win_game, COLOR_PAIR(PAIR_MYTK));
}

void attroff_mytk(void)
{
    wattroff(win_game, COLOR_PAIR(PAIR_MYTK));
}

void init_ui(void)
{
    initscr();
    curs_set(0);
    noecho();
    start_color();

    int starty, startx, height, width;
    height = MAP_HEIGHT;
    width = MAP_WIDTH * 2;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;

    win_game = newwin(height, width, starty, startx);

    attron_mytk();
    print_tank(&my_tank);
    attroff_mytk();

    box(win_game, 0, 0);
    refresh();
    wrefresh(win_game);

    keypad(stdscr, TRUE);
    keypad(win_game, TRUE);
}

void deinit_ui(void)
{
    endwin();
}
    
void print_tank(const tank *tk)
{
    int id = tk->id;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            map[tk->y + i][tk->x + j] = id;
            if (tank_pattern[tk->dir][i + 1][j + 1])
                PRINT_BLOCK(win_game, tk->y + i, tk->x + j);
        }
}

void erase_tank(const tank *tk)
{
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            map[tk->y + i][tk->x + j] = BLOCK_EMPTY;
            if (tank_pattern[tk->dir][i + 1][j + 1])
                ERASE_BLOCK(win_game, tk->y + i, tk->x + j);
        }
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

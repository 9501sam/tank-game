#include <ncurses.h>

#include "tank.h"

WINDOW *win_game;

void init_screen()
{
    initscr();
    curs_set(0);
    noecho();

    int starty, startx, height, width;
    height = GAME_HEIGHT;
    width = GAME_WIDTH * 2;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;
    win_game = newwin(height, width, starty, startx);
    box(win_game, 0, 0);
    refresh();
    wrefresh(win_game);
}


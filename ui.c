#include <ncurses.h>

#include "ui.h"
#include "game.h"

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
    mvwprintw(win_game, 3, 3, "%d %d", my_tank.x, my_tank.y);
    wrefresh(win_game);
}

int get_input(void)
{
    return wgetch(win_game);
}

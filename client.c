#include <ncurses.h>

#include "tank.h"

int main(int argc, char **argv)
{
    init_screen();

    connect();

    getch();
    endwin();
    return 0;
}

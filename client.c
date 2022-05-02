#include <stdlib.h>
#include <ncurses.h>

#include "tank.h"
#include "ui.h"
#include "macros.h"
#include "game.h"

int main(int argc, char **argv)
{
    if (atexit(deinit_ui))
        err_exit("atexit");

    init_ui();

    // connect();

    start_game();
    endwin();
    return 0;
}

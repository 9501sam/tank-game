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

static void attron_tank(int id)
{
    int pair_id = id + 1;
    int color_id = id + 2;
    init_pair(pair_id, color_id, COLOR_BLACK);
    wattron(win_game, COLOR_PAIR(pair_id));
    attron(COLOR_PAIR(pair_id));
}

static void attroff_tank(int id)
{
    int pair_id = id + 1;
    wattroff(win_game, COLOR_PAIR(pair_id));
    attroff(COLOR_PAIR(pair_id));
}

void init_ui(void)
{
    initscr();
    curs_set(0);
    noecho();
    nodelay(win_game, TRUE);
    start_color();

    int starty, startx, height, width;
    height = MAP_HEIGHT;
    width = MAP_WIDTH * 2;
    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;

    win_game = newwin(height, width, starty, startx);

    print_all_walls();
    print_tank(&my_tank);

    refresh();
    wrefresh(win_game);

    keypad(stdscr, TRUE);
    keypad(win_game, TRUE);
}

void deinit_ui(void)
{
    endwin();
}

void print_tank_info(const tank *tk)
{
    int id = tk->id;
    attron_tank(id);
    mvprintw(tk->id, 0, "player %d: (%d,%d), hp=%d, bullets=%d", tk->id + 1,
            tk->x, tk->y, tk->hp, tk->nblts);
    attroff_tank(id);
}

void erase_tank_info(const tank *tk)
{
    mvprintw(tk->id, 0, "                                                ");
}
    
void print_tank(const tank *tk)
{
    print_tank_info(tk);
    int id = tk->id;
    attron_tank(id);
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            map[tk->y + i][tk->x + j] = id;
            if (tank_pattern[tk->dir][i + 1][j + 1])
                PRINT_BLOCK(win_game, tk->y + i, tk->x + j);
        }
    attroff_tank(id);
}

void erase_tank(const tank *tk)
{
    erase_tank_info(tk);
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

void print_all_walls(void)
{
    for (int i = 0; i < MAP_HEIGHT; i++)
        for (int j = 0; j < MAP_WIDTH; j++)
            if (map[i][j] == BLOCK_WALL) {
                PRINT_BLOCK(win_game, i, j);
            }
    // box(win_game, 0, 0);
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
    case 'h':
        return INPUT_LEFT;
        break;
    case KEY_RIGHT:
    case 'l':
        return INPUT_RIGHT;
        break;
    case KEY_UP:
    case 'k':
        return INPUT_UP;
        break;
    case KEY_DOWN:
    case 'j':
        return INPUT_DOWN;
        break;
    case ' ':
    case 'x':
        return INPUT_SHOOT;
        break;
    case 'z':
        return INPUT_REFILL;
        break;
    case 'q':
    case 'Q':
        return INPUT_QUIT;
        break;
    }
    return INPUT_INVALID;
}

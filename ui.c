#include "tankio.h"

#define PRINT_CH(window, pos_y, pos_x, ch)                 \
    do {                                                   \
        mvwaddch((window), (pos_y), ((pos_x) << 1), (ch)); \
        waddch((window), (ch));                            \
    } while (0)

#define PRINT_BLOCK(window, pos_y, pos_x) \
    PRINT_CH(window, pos_y, pos_x, ACS_CKBOARD)

#define ERASE_BLOCK(window, pos_y, pos_x) \
    PRINT_CH(window, pos_y, pos_x, ' ')

WINDOW *win_game, *win_info, *win_mytank_info;

static void attron_tank(int id)
{
    int pair_id = id + 1;
    int color_id = id + 1;
    init_pair(pair_id, color_id, COLOR_BLACK);
    wattron(win_game, COLOR_PAIR(pair_id));
    wattron(win_info, COLOR_PAIR(pair_id));
    wattron(win_mytank_info, COLOR_PAIR(pair_id));
    attron(COLOR_PAIR(pair_id));
}

static void attroff_tank(int id)
{
    int pair_id = id + 1;
    wattroff(win_game, COLOR_PAIR(pair_id));
    wattroff(win_info, COLOR_PAIR(pair_id));
    wattroff(win_mytank_info, COLOR_PAIR(pair_id));
    attroff(COLOR_PAIR(pair_id));
}

void init_ui(void)
{
    initscr();
    curs_set(0);
    noecho();
    start_color();

    // win_game
    int game_starty, game_startx, game_height, game_width;
    game_height = MAP_HEIGHT;
    game_width = MAP_WIDTH * 2;
    game_starty = (LINES - game_height) / 2;
    game_startx = (COLS - game_width) / 2 - 20;
    win_game = newwin(game_height, game_width, game_starty, game_startx);
    keypad(win_game, TRUE);

    // win_info
    int info_starty, info_startx, info_height, info_width;
    info_height = MAX_USERS + 2;
    info_width = 40;
    info_starty = game_starty + 8;
    info_startx = game_startx + game_width + 1;
    win_info = newwin(info_height, info_width, info_starty, info_startx);
    box(win_info, 0, 0);

    // win_mytank_info
    int mtkinfo_starty, mtkinfo_startx, mtkinfo_height, mtkinfo_width;
    mtkinfo_height = 7;
    mtkinfo_width = 40;
    mtkinfo_starty = game_starty;
    mtkinfo_startx = game_startx + game_width + 1;
    win_mytank_info = newwin(mtkinfo_height, mtkinfo_width, mtkinfo_starty, mtkinfo_startx);
    box(win_mytank_info, 0, 0);
    mvwprintw(win_mytank_info, 0, 0, "move: arrow keys, shoot: x, refill: z ");

    print_all_walls();
    print_tank(&my_tank);

    refresh();
    wrefresh(win_game);
    wrefresh(win_info);
    wrefresh(win_mytank_info);
}

void deinit_ui(void)
{
    endwin();
}

void print_tank_info(const tank *tk)
{
    int id = tk->id;
    attron_tank(id);
    mvwprintw(win_info, tk->id + 1, 1,
            "player %d: (%d,%d), hp=%d, bullets=%d", tk->id + 1,
            tk->x, tk->y, tk->hp, tk->nblts);
    if (id == my_tank.id) {
        mvwprintw(win_mytank_info, 2, 1, "hp:");
        for (int i = 0; i < DEFAULT_HP; i++) {
            if (i < my_tank.hp)
                PRINT_BLOCK(win_mytank_info, 2, i * 2 + 3);
            else
                ERASE_BLOCK(win_mytank_info, 2, i * 2 + 3);
        }
        mvwprintw(win_mytank_info, 4, 1, "bullets:");
        for (int i = 0; i < NUM_BULLETS; i++) {
            if (i < my_tank.nblts)
                PRINT_BLOCK(win_mytank_info, 4, i * 2 + 5);
            else
                ERASE_BLOCK(win_mytank_info, 4, i * 2 + 5);
        }
    }
    attroff_tank(id);
}

void erase_tank_info(const tank *tk)
{
    mvwprintw(win_info,
            tk->id + 1, 1, "                                   ");
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
}

void refresh_screen(void)
{
    refresh();
    wrefresh(win_game);
    wrefresh(win_info);
    wrefresh(win_mytank_info);
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

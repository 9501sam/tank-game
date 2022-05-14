#include "tankio.h"

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

static bool check_tank(tank *tk)
{
    int x = tk->x;
    int y = tk->y;
    if (x < 2)
        return false;
    if (x > MAP_WIDTH - 3)
        return false;
    if (y < 2)
        return false;
    if (y > MAP_HEIGHT - 3)
        return false;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
            if ((map[y + i][x + j] != BLOCK_EMPTY) &&
                    (map[y + i][x + j] != tk->id))
                return false;
    return true;
}

bool goforward(tank *tk)
{
    tank newtk = *tk;
    switch (newtk.dir) {
    case LEFT:
        newtk.x--;
        break;
    case RIGHT:
        newtk.x++;
        break;
    case UP:
        newtk.y--;
        break;
    case DOWN:
        newtk.y++;
        break;
    default:
        return false;
    }
    if (!check_tank(&newtk))
        return false;
    erase_tank(tk);
    tk->x = newtk.x;
    tk->y = newtk.y;
    attron_tank(tk->id);
    print_tank(tk);
    attroff_tank(tk->id);
    refresh_screen();
    struct package pkg = {.kind = TANK, .data = {*tk}};
    if ((send(client_sock, &pkg, sizeof(pkg), 0)) < 0)
        perror("goforward\n");
    return true;
}

bool turn(tank *tk, DIRECTION dir)
{
    if (tk->dir == dir)
        return false;
    erase_tank(tk);
    tk->dir = dir;
    attron_tank(tk->id);
    print_tank(tk);
    attroff_tank(tk->id);
    refresh_screen();
    struct package pkg = {.kind = TANK, .data.tk = *tk};
    if ((send(client_sock, &pkg, sizeof(pkg), 0)) < 0)
        perror("turn\n");
    return true;
}

bool attacked(tank *tk)
{
    return true;
}

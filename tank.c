#include "tankio.h"

bool goforward(tank *tk)
{
    tank oldtk = {
        .x = tk->x,
        .y = tk->y,
        .dir = tk->dir,
    };
    switch (tk->dir) {
    case LEFT:
        if (tk->x - 1 < 2)
            return false;
        tk->x--;
        break;
    case RIGHT:
        if (tk->x + 1 > MAP_WIDTH - 3)
            return false;
        tk->x++;
        break;
    case UP:
        if (tk->y - 1 < 2)
            return false;
        tk->y--;
        break;
    case DOWN:
        if (tk->y + 1 > MAP_HEIGHT - 3)
            return false;
        tk->y++;
        break;
    default:
        return false;
    }
    erase_tank(&oldtk);
    print_tank(tk);
    struct action act = {5};
    send(client_sock, &act, sizeof(act), 0);
    return true;
}

bool turn(tank *tk, DIRECTION dir)
{
    tank oldtk = {
        .x = tk->x,
        .y = tk->y,
        .dir = tk->dir,
    };
    if (tk->dir == dir)
        return false;
    tk->dir = dir;
    erase_tank(&oldtk);
    print_tank(tk);
    return true;
}


bool attacked(tank *tk)
{
    return true;
}

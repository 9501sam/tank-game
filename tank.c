#include "tank.h"

bool goforward(tank *tk)
{
    switch (tk->dir) {
    case LEFT:
        if (tk->x - 1 < 2)
            return false;
        tk->x--;
        break;
    case RIGHT:
        if (tk->x + 1 > MAP_HEIGHT - 3)
            return false;
        tk->x++;
        break;
    case UP:
        if (tk->y - 1 < 2)
            return false;
        tk->y--;
        break;
    case DOWN:
        if (tk->y + 1 > MAP_WIDTH - 3)
            return false;
        tk->y++;
        break;
    default:
        return false;
    }
    return true;
}

bool turn(tank *tk, DIRECTION dir)
{
    if (tk->dir == dir)
        return false;
    tk->dir = dir;
    return true;
}

#include "tank.h"

void goforward(tank *tk)
{
    if (tk->dir == LEFT)
        tk->x--;
    else if (tk->dir == RIGHT)
        tk->x++;
    else if (tk->dir == UP)
        tk->y--;
    else if (tk->dir == DOWN)
        tk->y++;
}

void turn(tank * tk, DIRECTION dir)
{
    tk->dir = dir;
}

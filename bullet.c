#include <ncurses.h>
#include <time.h>
#include <pthread.h>

#include "tankio.h"

static bool bullets_move(bullet *blt)
{
    switch (blt->dir) {
    case LEFT:
        if (blt->x - 1 < 1)
            return false;
        blt->x--;
        break;
    case RIGHT:
        if (blt->x + 1 > MAP_WIDTH - 2)
            return false;
        blt->x++;
        break;
    case UP:
        if (blt->y - 1 < 1)
            return false;
        blt->y--;
        break;
    case DOWN:
        if (blt->y + 1 > MAP_HEIGHT - 2)
            return false;
        blt->y++;
        break;
    default:
        return false;
    }
    return true;
}

void *fire(void *arg)
{
    bullet *blt = (bullet *) arg;

    bool ret = true;
    while (ret) {
        pthread_mutex_lock(&lock);
        
        ret = bullets_move(blt);
        refresh_screen();

        pthread_mutex_unlock(&lock);
        napms(12);
    }

    return NULL;
}

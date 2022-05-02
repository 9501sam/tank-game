#include "bullet.h"
#include "game.h"
#include "ui.h"

#include <ncurses.h>
#include <time.h>
#include <pthread.h>

bullet   bullets[MAX_BULLETS];

static bool bullets_move(void)
{
    bullet *blt = &bullets[0];

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
    struct thread_data *data = (struct thread_data *)arg;

    pthread_mutex_lock(&data->lock);
    bullets[0].x = my_tank.x;
    bullets[0].y = my_tank.y;
    bullets[0].dir = my_tank.dir;
    pthread_mutex_unlock(&data->lock);

    bool ret = true;
    while (ret) {
        pthread_mutex_lock(&data->lock);
        
        ret = bullets_move();
        refresh_screen();

        pthread_mutex_unlock(&data->lock);
        napms(12);
    }

    pthread_mutex_lock(&data->lock);
    bullets[0].x = 0;
    bullets[0].y = 0;
    pthread_mutex_unlock(&data->lock);
    return NULL;
}

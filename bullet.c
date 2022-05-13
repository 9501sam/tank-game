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
    bool is_moved = true;
    pthread_mutex_lock(&lock);
    print_bullet(blt);
    refresh_screen();
    pthread_mutex_unlock(&lock);
    while (is_moved) {
        pthread_mutex_lock(&lock);
        erase_bullet(blt);
        is_moved = bullets_move(blt);
        print_bullet(blt);
        refresh_screen();
        pthread_mutex_unlock(&lock);
        napms(12);
    }
    free(blt);
    return NULL;
}

void fire_thread_create(tank *tk)
{
    pthread_t tid;
    bullet *blt = (bullet *) malloc(sizeof(bullet));
    blt->x = tk->x;
    blt->y = tk->y;
    blt->dir = tk->dir;
    if (!bullets_move(blt))
        return;
    if (!bullets_move(blt))
        return;
    pthread_create(&tid, NULL, fire, (void *) blt);
}

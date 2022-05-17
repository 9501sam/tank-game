#include "tankio.h"

static bool check_bullet(bullet *blt)
{
    if (blt->x < 1)
        return false;
    if (blt->x > MAP_WIDTH - 2)
        return false;
    if (blt->y < 1)
        return false;
    if (blt->y > MAP_HEIGHT - 2)
        return false;
    if (map[blt->y][blt->x] == my_tank.id) { // my_tank attacked
        my_tank_attacked();
        return false;
    }
    if (map[blt->y][blt->x] != BLOCK_EMPTY)
        return false;
    return true;
}

static bool bullets_move(bullet *blt)
{
    bullet newblt = *blt;
    switch (newblt.dir) {
    case LEFT:
        newblt.x--;
        break;
    case RIGHT:
        newblt.x++;
        break;
    case UP:
        newblt.y--;
        break;
    case DOWN:
        newblt.y++;
        break;
    default:
        return false;
    }
    blt->x = newblt.x;
    blt->y = newblt.y;
    return true;
}

void *shoot(void *arg)
{
    bullet *blt = (bullet *) arg;
    bool is_moved = true;
    if (!bullets_move(blt)) {
        free(blt);
        return NULL;
    }
    if (!bullets_move(blt)) {
        free(blt);
        return NULL;
    }
    pthread_mutex_lock(&lock);
    if ((is_moved = check_bullet(blt))) {
        print_bullet(blt);
        refresh_screen();
    }
    pthread_mutex_unlock(&lock);
    if (!is_moved) {
        free(blt);
        return NULL;
    }
    while (is_moved) {
        napms(BULLET_DELAY);
        pthread_mutex_lock(&lock);
        erase_bullet(blt);
        bullets_move(blt);
        is_moved = check_bullet(blt);
        if (is_moved)
            print_bullet(blt);
        refresh_screen();
        pthread_mutex_unlock(&lock);
    }
    free(blt);
    return NULL;
}

void shoot_thread_create(tank *tk)
{
    pthread_t tid;
    bullet *blt = (bullet *) malloc(sizeof(bullet));
    blt->x = tk->x;
    blt->y = tk->y;
    blt->dir = tk->dir;
    erase_tank_info(tk);
    print_tank_info(tk);
    refresh_screen();
    pthread_create(&tid, NULL, shoot, (void *) blt);
}

void my_tank_shoot(void)
{
    struct packet pkt = {
        .kind = SHOOT,
        .data.tk = my_tank,
    };
    if (send_packet(client_sock, &pkt) == -1)
        perror("send");
    shoot_thread_create(&my_tank);
}

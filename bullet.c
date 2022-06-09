#include "tankio.h"

static bool check_bullet(bullet *blt)
{
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
    pthread_detach(pthread_self());

    int id = *(int *) arg;
    free(arg);
    tank *tk = NULL;
    if (id == my_tank.id)
        tk = &my_tank;
    else
        tk = &enemies[id];
    bullet blt = {.x = tk->x, .y = tk->y, .dir = tk->dir};
    struct timespec rem, req = {.tv_sec = 0, .tv_nsec = BULLET_DELAY};
    bool is_moved = true;
    if (!bullets_move(&blt)) {
        return NULL;
    }
    if (!bullets_move(&blt))
        return NULL;
    pthread_mutex_lock(&lock);
    if ((is_moved = check_bullet(&blt))) {
        print_bullet(&blt);
        refresh_screen();
    }
    pthread_mutex_unlock(&lock);
    if (!is_moved)
        return NULL;
    while (is_moved) {
        nanosleep(&req, &rem);
        pthread_mutex_lock(&lock);
        erase_bullet(&blt);
        bullets_move(&blt);
        is_moved = check_bullet(&blt);
        if (is_moved)
            print_bullet(&blt);
        refresh_screen();
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void shoot_thread_create(int id)
{
    pthread_t tid;
    tank *tk = NULL;
    if (id == my_tank.id)
        tk = &my_tank;
    else
        tk = &enemies[id];
    erase_tank_info(tk);
    print_tank_info(tk);
    refresh_screen();

    int *arg = malloc(sizeof(int));
    *arg = id;
    pthread_create(&tid, NULL, shoot, (void *) arg);
}

void my_tank_shoot(void)
{
    struct packet pkt = {
        .kind = SHOOT,
        .data.id = my_tank.id,
    };
    if (send_packet(client_sock, &pkt) == -1)
        perror("send");
    shoot_thread_create(my_tank.id);
}

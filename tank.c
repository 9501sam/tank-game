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
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
            if ((map[y + i][x + j] != BLOCK_EMPTY) &&
                    (map[y + i][x + j] != tk->id))
                return false;
    return true;
}

static bool goforward(tank *tk)
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
    tk->x = newtk.x;
    tk->y = newtk.y;
    return true;
}

static bool turn(tank *tk, direction_t dir)
{
    if (tk->dir == dir)
        return false;
    tk->dir = dir;
    return true;
}

bool my_tank_move(direction_t dir)
{
    bool is_moved;
    tank oldtk = my_tank;
    is_moved = turn(&my_tank, dir);
    is_moved = is_moved | goforward(&my_tank);
    if (is_moved) {
        erase_tank(&oldtk);
        print_tank(&my_tank);
        refresh_screen();

        struct packet pkt = {.kind = TANK, .data.tk = my_tank};
        if ((send_packet(client_sock, &pkt)) < 0)
            perror("my_tank_move\n");
    }
    return is_moved;
}

void my_tank_attacked(void)
{
    if (my_tank.hp > 1) {
        my_tank.hp--;
        erase_tank_info(&my_tank);
        print_tank_info(&my_tank);
        refresh_screen();
        struct packet pkt = {
            .kind = ATTACKED,
            .data.id = my_tank.id,
        };
        if (send_packet(client_sock, &pkt) == -1)
            perror("send");
        return;
    } else {    // my_tank died
        exit(EXIT_SUCCESS);
    }
}

void my_tank_refill(void)
{
    if (my_tank.nblts == NUM_BULLETS)
        return;
    struct packet pkt = {.kind = REFILL, .data.id = my_tank.id};
    my_tank.nblts = NUM_BULLETS;
    erase_tank_info(&my_tank);
    print_tank_info(&my_tank);
    refresh_screen();
    if ((send_packet(client_sock, &pkt)) < 0)
        perror("my_tank_refill\n");
}

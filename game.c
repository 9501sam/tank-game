#include "tankio.h"

/* these global variables should be sync using lock */
tank             enemies[MAX_USERS];
tank             my_tank;
int              client_sock;
int              map[MAP_HEIGHT][MAP_WIDTH];
pthread_mutex_t  lock = PTHREAD_MUTEX_INITIALIZER;

static void init_game(void)
{
    memset(map, BLOCK_EMPTY, sizeof(map));
    for (int i = 0; i < MAX_USERS; i++)
        enemies[i].id = NOT_USED;
}

bool add_enemy(tank *tk)
{
    if (enemies[tk->id].id != -1)
        return false;
    enemies[tk->id] = *tk;
    return true;
}

bool del_enemy(int id)
{
    if (enemies[id].id == -1)
        return false;
    enemies[id].id = -1;
    return true;
}

static void main_loop(void)
{
    input_t in;
    while (1) {
        in = get_input();
        if (in == INPUT_INVALID)
            continue;
        pthread_mutex_lock(&lock);
        switch (in) {
        case INPUT_LEFT:
            turn(&my_tank, LEFT);
            goforward(&my_tank);
            break;
        case INPUT_RIGHT:
            turn(&my_tank, RIGHT);
            goforward(&my_tank);
            break;
        case INPUT_UP:
            turn(&my_tank, UP);
            goforward(&my_tank);
            break;
        case INPUT_DOWN:
            turn(&my_tank, DOWN);
            goforward(&my_tank);
            break;
        case INPUT_SHOOT:
            if (my_tank.nblts > 0) {
                my_tank.nblts--;
                my_tank_shoot();
            }
            break;
        case INPUT_REFILL:
            my_tank_refill();
            break;
        case INPUT_QUIT:
            return;
            break;
        case INPUT_INVALID:
            break;
        }
        pthread_mutex_unlock(&lock);
    }
}

static void *recv_thread(void *arg)
{
    struct packet pkt;
    int id;

    while (recv_packet(client_sock, &pkt)) {
        pthread_mutex_lock(&lock);
        switch (pkt.kind) {
        case NEW_TANK:
            id = pkt.data.tk.id;
            add_enemy(&pkt.data.tk);
            print_tank(&pkt.data.tk);
            refresh_screen();
            break;
        case TANK:
            id = pkt.data.tk.id;
            tank oldtk = enemies[id];
            enemies[id] = pkt.data.tk;
            erase_tank(&oldtk);
            print_tank(&enemies[id]);
            refresh_screen();
            break;
        case SHOOT:
            id = pkt.data.tk.id;
            enemies[id] = pkt.data.tk;
            erase_tank_info(&enemies[id]);
            print_tank_info(&enemies[id]);
            refresh_screen();
            shoot_thread_create(&pkt.data.tk);
            break;
        case REFILL:
            id = pkt.data.id;
            enemies[id].nblts = NUM_BULLETS;
            erase_tank_info(&enemies[id]);
            print_tank_info(&enemies[id]);
            refresh_screen();
            break;
        case ATTACKED:
            id = pkt.data.id;
            enemies[id].hp--;
            print_tank_info(&enemies[id]);
            refresh_screen();
            break;
        case DIE:
            id = pkt.data.id;
            tank dietk = enemies[id];
            if (dietk.id != NOT_USED) {
                erase_tank(&dietk);
                refresh_screen();
            }
            break;
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void start_game(void)
{
    pthread_t t1;

    init_game();
    init_ui();
    pthread_create(&t1, NULL, recv_thread, NULL);
    main_loop();
    pthread_join(t1, NULL);
}

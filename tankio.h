#ifndef TANKIO_H
#define TANKIO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>


#define err_exit(msg)       \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define MAX_USERS    6
#define MAX_FD       100

#define MAP_HEIGHT   16
#define MAP_WIDTH    24

#define TANK_SIZE    3
#define DEFAULT_HP   3
#define NUM_BULLETS  5
#define BULLET_DELAY 30
#define REFILL_DELAY 2000

#define BLOCK_EMPTY  -1
#define NOT_USED     -1

#define PAIR_MYTK    1
#define COLOR_MYTK   COLOR_GREEN

// direction
typedef enum {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NUM_DIR,
} DIRECTION;

///*** tank ***///
typedef struct {
    uint16_t x;
    uint16_t y;
    DIRECTION dir;
    int hp;
    int nblts;
    int id;
} tank;

bool goforward(tank *);
bool turn(tank *, DIRECTION);
void my_tank_attacked();
void my_tank_refill(void);

///*** bullet ***///
typedef struct {
    int x;
    int y;
    DIRECTION dir;
} bullet;

void *shoot(void *);
void shoot_thread_create(tank *tk);
void my_tank_shoot(void);

///*** ui ***///
typedef enum {
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_SHOOT,
    INPUT_REFILL,
    INPUT_QUIT,
    INPUT_INVALID,
} input_t;

void init_ui(void);
void deinit_ui(void);
void erase_tank(const tank *);
void print_tank_info(const tank *);
void erase_tank_info(const tank *);
void print_tank(const tank *);
void erase_bullet(const bullet *);
void print_bullet(const bullet *);
void refresh_screen(void);
void attron_tank(int);
void attroff_tank(int);
input_t get_input(void);

///*** network ***///
struct package {
    enum {NEW_TANK, TANK, SHOOT, REFILL, ATTACKED, DIE} kind;
    union {
        tank   tk;
        int8_t refill_id;
        int8_t attacked_id;
        int8_t die_id;
    } data;
};

void *recv_thread(void *);

///*** game ***///
/* these global variables should be sync using lock */
extern tank             enemies[MAX_USERS];
extern tank             my_tank;
extern int              client_sock;
extern int              map[MAP_HEIGHT][MAP_WIDTH];
extern pthread_mutex_t  lock;

extern const int tank_pattern[NUM_DIR][TANK_SIZE][TANK_SIZE];

bool add_enemy(tank *);
bool del_enemy(int);
void start_game(void);

#endif /* TANKIO_H */

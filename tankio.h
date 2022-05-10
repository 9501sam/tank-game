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
#include <ncurses.h>

#define err_exit(msg)       \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define MAX_USERS   10
#define MAX_FD      100
#define MAX_BULLETS 100

#define MAP_HEIGHT  16
#define MAP_WIDTH   24

#define TANK_SIZE   3
#define DEFAULT_PH  3

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
    int ph;
    int id;
} tank;

bool goforward(tank *);
bool turn(tank *, DIRECTION);
bool attacked(tank *);

///*** bullet ***///
typedef struct {
    int x;
    int y;
    DIRECTION dir;
} bullet;

void *fire(void *);

///*** ui ***///
typedef enum {
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_FIRE,
    INPUT_QUIT,
    INPUT_INVALID,
} input_t;

void init_ui(void);
void deinit_ui(void);
void erase_tank(const tank *);
void print_tank(const tank *);
void erase_bullet(const bullet *);
void print_bullet(const bullet *);
void refresh_screen(void);
input_t get_input(void);

///*** network ***///
struct package {
    enum {NEW_TANK, TANK, BULLET, ATTACKED, DIE} kind;
    union {
        tank    tk;
        bullet  blt;
        int     attacked_id;
        int     die_id;
    } data;
};

void *recv_thread(void *);

///*** game ***///
/* these global variable should be syc using lock */
extern tank             enemies[MAX_USERS];
extern tank             my_tank;
extern int              client_sock;
extern pthread_mutex_t  lock;

bool add_enemy(tank *);
bool del_enemy(int);
void start_game(int);

#endif /* TANKIO_H */

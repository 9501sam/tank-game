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

#define err_exit(msg)       \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define MAX_USERS   10
#define MAX_BULLETS 100

#define MAP_HEIGHT  16
#define MAP_WIDTH   24

#define TANK_SIZE   3

// direction
typedef enum {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NUM_DIR,
} DIRECTION;

struct thread_data {
    pthread_mutex_t lock;
    pthread_t thread_id;
};

///*** tank ***///
typedef struct {
    uint16_t x;
    uint16_t y;
    DIRECTION dir;
} tank;

bool goforward(tank *);
bool turn(tank *, DIRECTION);
bool attacked(tank *);

///*** tank ***///
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

void erase_tank(tank);
void print_tank(tank);

void erase_bullet(bullet);
void print_bullet(bullet);

void refresh_screen(void);
input_t get_input(void);

///*** network ***///
struct action {
};

void *network_thread(void *);

///*** game ***///
/* these global variable should be syc using lock */
extern tank             enemies[MAX_USERS];
extern tank             my_tank;
extern pthread_mutex_t  lock;

void start_game(int);

#endif /*TANKIO_H*/

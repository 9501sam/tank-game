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
#include <assert.h>
#include <netdb.h>

#define err_exit(msg)       \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define MAX_USERS    6
#define MAX_FD       100

#define MAP_HEIGHT   31
#define MAP_WIDTH    40

#define NOT_USED     -1
#define BLOCK_EMPTY  -1
#define BLOCK_WALL   -2
#define _            BLOCK_EMPTY
#define W            BLOCK_WALL
#define INIT_MAP                                                                            \
{                                                                                           \
        {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,}, \
        {W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,}, \
        {W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,}, \
        {W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,}, \
        {W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,W,W,W,W,W,W,W,W,W,W,W,W,W,W,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,W,W,W,W,W,W,W,W,W,_,_,_,_,_,_,_,_,_,_,W,W,W,W,W,W,W,W,W,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,W,_,_,_,_,_,_,_,W,W,W,W,W,W,W,W,W,W,W,W,W,W,_,_,_,_,_,_,_,W,_,_,_,_,W,}, \
        {W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,}, \
        {W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,}, \
        {W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,}, \
        {W,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,W,}, \
        {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,}, \
}

#define TANK_SIZE    3
#define DEFAULT_HP   3
#define NUM_BULLETS  5
#define BULLET_DELAY 30000000
#define REFILL_DELAY 2000

// direction
typedef uint8_t direction_t;
#define LEFT    (direction_t) 0
#define RIGHT   (direction_t) 1
#define UP      (direction_t) 2
#define DOWN    (direction_t) 3
#define NUM_DIR (direction_t) 4

///*** tank ***///
typedef struct {
    uint16_t x;
    uint16_t y;
    direction_t dir;
    int8_t hp;
    int8_t nblts;
    int8_t id;
} tank;

bool my_tank_move(direction_t);
void my_tank_attacked(void);
void my_tank_refill(void);

///*** bullet ***///
typedef struct {
    int x;
    int y;
    direction_t dir;
} bullet;

void *shoot(void *);
void shoot_thread_create(int id);
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
void print_all_walls(void);
void refresh_screen(void);
input_t get_input(void);

///*** network ***///
typedef uint8_t packet_kind;
#define NEW_TANK (packet_kind) 0
#define TANK     (packet_kind) 1
#define SHOOT    (packet_kind) 2
#define REFILL   (packet_kind) 3
#define ATTACKED (packet_kind) 4
#define DIE      (packet_kind) 5

struct packet {
    packet_kind kind;
    union {
        tank   tk;
        int8_t id;
    } data;
};

int recv_packet(int fd, struct packet *pkt);
int send_packet(int fd, struct packet *pkt);

///*** game ***///
/* these global variables should be sync using lock */
extern tank             enemies[MAX_USERS];
extern tank             my_tank;
extern int              client_sock;
extern int              map[MAP_HEIGHT][MAP_WIDTH];
extern pthread_mutex_t  lock;

extern const int tank_pattern[NUM_DIR][TANK_SIZE][TANK_SIZE];

void start_game(void);

#endif /* TANKIO_H */

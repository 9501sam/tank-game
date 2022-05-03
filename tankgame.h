#ifndef __TANKGAME_H__
#define __TANKGAME_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define err_exit(msg)       \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define MAX_USERS   10
#define MAX_BULLETS 100

#define MAP_HEIGHT  20
#define MAP_WIDTH   20

// direction
typedef enum {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NUM_DIR,
} DIRECTION;

#define TANK_SIZE   (uint8_t) 3

struct thread_data {
    pthread_mutex_t lock;
    pthread_t thread_id;
};

#endif /*__TANKGAME_H__*/

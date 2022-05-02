#ifndef __MACROS_H__
#define __MACROS_H__

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

#define MAP_HEIGHT  32
#define MAP_WIDTH   32

// direction
typedef uint8_t DIRECTION;

#define LEFT    (uint8_t) 0x1
#define RIGHT   (uint8_t) 0x2
#define UP      (uint8_t) 0x4
#define DOWN    (uint8_t) 0x8

#endif /*__MACROS_H__*/

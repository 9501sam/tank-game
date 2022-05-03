#ifndef __BULLET_H__
#define __BULLET_H__

#include "tankgame.h"

typedef struct {
    int x;
    int y;
    DIRECTION dir;
} bullet;

void *fire(void *);

#endif /*__BULLET_H__*/

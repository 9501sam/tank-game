#ifndef __BULLET_H__
#define __BULLET_H__

#include "macros.h"

typedef struct {
    int x;
    int y;
    DIRECTION dir;
} bullet;

void fire(bullet *);

#endif /*__BULLET_H__*/

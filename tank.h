#ifndef __TANK_H__
#define __TANK_H__

#include <stdint.h>
#include <stdbool.h>

#include "tankgame.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    DIRECTION dir;
} tank;

bool goforward(tank *);
bool turn(tank *, DIRECTION);
bool attacked(tank *);

#endif /*__TANK_H__*/

#ifndef __TANK_H__
#define __TANK_H__

#include <stdint.h>

#include "macros.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    DIRECTION dir;
} tank;

void goforward(tank *);
void turn(tank *, DIRECTION);

#endif /*__TANK_H__*/

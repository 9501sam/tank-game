#ifndef __MAP_H__
#define __MAP_H__

#include "tankgame.h"

typedef struct {
    char m[MAP_HEIGHT][MAP_WIDTH];
} map;

map *newmap(void);

#endif /*__MAP_H__*/

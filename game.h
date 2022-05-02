#ifndef __GAME_H__
#define __GAME_H__

#include "macros.h"
#include "map.h"
#include "tank.h"
#include "bullet.h"

extern map      *game_map;
extern tank     *tanks[MAX_USERS];
extern tank     my_tank;
extern bullet   *bullets[MAX_BULLETS];

void start_game(void);

#endif /*__GAME_H__*/

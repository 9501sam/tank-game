#ifndef __GAME_H__
#define __GAME_H__

#include "tankgame.h"
#include "tank.h"
#include "bullet.h"

// extern tank     *tanks[MAX_USERS];
extern tank     my_tank;
extern bullet   bullets[MAX_BULLETS];

void start_game(void);

#endif /*__GAME_H__*/

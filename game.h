#ifndef __GAME_H__
#define __GAME_H__

#include "tankgame.h"
#include "tank.h"
#include "bullet.h"
#include "network.h"

// extern tank     *tanks[MAX_USERS];
extern tank     my_tank;
extern bullet   bullets[MAX_BULLETS];
extern int sockfd;

void start_game(int);

#endif /*__GAME_H__*/

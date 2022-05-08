#ifndef __GAME_H__
#define __GAME_H__

#include "tankgame.h"
#include "tank.h"
#include "bullet.h"
#include "network.h"

/* these global variable should be syc using lock */
extern tank             enemies[MAX_USERS];
extern tank             my_tank;
extern pthread_mutex_t  lock;

void start_game(int);

#endif /*__GAME_H__*/

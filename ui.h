#ifndef __UI_H__
#define __UI_H__

#include "tankgame.h"

typedef enum {
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_FIRE,
    INPUT_QUIT,
    INPUT_INVALID,
} input_t;

void init_ui(void);
void deinit_ui(void);

void erase_tank(tank);
void print_tank(tank);

void erase_bullet(bullet);
void print_bullet(bullet);

void refresh_screen(void);
input_t get_input(void);

#endif /*__UI_H__*/

#ifndef GAP_RUSH_H
#define GAP_RUSH_H

#define USER_WIDTH 8
#define ROCK_WIDTH 128         
#define ROCK_HOLE_SIZE ((USER_WIDTH * 2) + 1)

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.h"
#include "buttons.h"
#include "rng.h"

typedef struct {
    uint8_t x;
    uint8_t y;
} Pos;

typedef struct {
    uint8_t shape[USER_WIDTH];
    Pos old_pos;
    Pos pos;
} User;

typedef struct {
    uint8_t hole_x;
    Pos old_pos;
    Pos pos;
} Rock;

void play_gap_rush();

#endif
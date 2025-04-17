#ifndef GAP_RUSH_H
#define GAP_RUSH_H

#define USER_WIDTH 8
#define ROCK_WIDTH 128         
#define ROCK_HOLE_SIZE (USER_WIDTH * 2)

#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>

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
static void init_game();
static void generate_rock();
static void update_user();
static void update_rock();
static void update_level();
static void check_collision();

#endif
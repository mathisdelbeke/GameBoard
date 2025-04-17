#ifndef SPACE_INVADER_H
#define SPACE_INVADER_H

#define USER_WIDTH 8
#define ROCK_WIDTH 128         // SCREEN WIDTH...
#define ROCK_HOLE (USER_WIDTH * 2)

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
    uint8_t hole_pos;
    Pos old_pos;
    Pos pos;
} Rock;

void play_space_invader();
void init_game();
void generate_rock();
void update_user();
void update_rock();
void check_level();
void check_collision();

#endif
#ifndef SPACE_INVADER_H
#define SPACE_INVADER_H

#define USER_WIDTH 8
#define ROCKS_WIDTH 12
#define ACTIVE_ROCKS 3

#include <avr/io.h>
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
    uint8_t shape[ROCKS_WIDTH];
    Pos old_pos;
    Pos pos;
} Rock;

void play_space_invader();
void update_user();
void generate_rock();

#endif
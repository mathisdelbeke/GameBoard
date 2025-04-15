#ifndef SPACE_INVADER_H
#define SPACE_INVADER_H

#define USER_WIDTH 8

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

void space_invader_start();
void update_user();

#endif
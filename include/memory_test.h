#ifndef MEMORY_TEST_H
#define MEMORY_TEST_H

#include <avr/io.h>
#include <util/delay.h>

#include "display.h"
#include "buttons.h"

typedef enum {
    DIRECTION_LEFT = 0,
    DIRECTION_RIGHT = 1,
    DIRECTION_UP = 0,
    DIRECTION_DOWN = 1
} Direction;

void play_memory_test();

#endif
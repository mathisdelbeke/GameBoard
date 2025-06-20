#ifndef SNAKE_H
#define SNAKE_H

#include <avr/io.h>
#include <util/delay.h>

#include "display.h"
#include "buttons.h"
#include "rng.h"

void play_snake();
void snake_timer_hit();

#endif
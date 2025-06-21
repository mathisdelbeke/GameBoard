#ifndef GAP_RUSH_H
#define GAP_RUSH_H

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.h"
#include "buttons.h"
#include "rng.h"

void play_gap_rush();
void gap_rush_timer_hit();

#endif
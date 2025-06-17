#ifndef BUTTONS_H
#define BUTTONS_H

#include <avr/io.h>
#include <avr/interrupt.h>

#define BTTN1 0
#define BTTN2 1
#define BTTN3 2
#define BTTN4 3

void bttns_init();
void init_debounce_timer();
uint8_t bttns_read();

#endif
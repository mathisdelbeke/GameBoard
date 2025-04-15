#ifndef BUTTONS_H
#define BUTTONS_H

#define BTTN1 0
#define BTTN2 1
#define BTTN3 2
#define BTTN4 3

#include <avr/io.h>
#include <avr/interrupt.h>

void bttns_init();
uint8_t bttns_read();
void init_debounce_timer();

#endif
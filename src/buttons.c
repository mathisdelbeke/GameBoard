#include "buttons.h"

#define BTTN1 PD6
#define BTTN2 PC3
#define BTTN3 PB2
#define BTTN4 PC0

static uint8_t bttns_states;

void bttns_init() {
    // Set pins as input
    DDRD &= ~(1 << BTTN1);
    DDRB &= ~(1 << BTTN3);
    DDRC &= ~((1 << BTTN2) | (1 << BTTN4));

    // Enable pull-up resistors
    PORTD |= (1 << BTTN1);
    PORTB |= (1 << BTTN3);
    PORTC |= ((1 << BTTN2) | (1 << BTTN4));
}

uint8_t bttns_read() {
    bttns_states = 0;

    // Active LOW, so invert the bit logic
    if (!(PIND & (1 << BTTN1))) bttns_states |= (1 << 0);
    if (!(PINC & (1 << BTTN2))) bttns_states |= (1 << 1);
    if (!(PINB & (1 << BTTN3))) bttns_states |= (1 << 2);
    if (!(PINC & (1 << BTTN4))) bttns_states |= (1 << 3);

    return bttns_states;
}

#include "buttons.h"

#define BTTN1_PIN PD6
#define BTTN2_PIN PC3
#define BTTN3_PIN PB2
#define BTTN4_PIN PC0

static uint8_t bttns_states;

void bttns_init() {
    // Set pins as input
    DDRD &= ~(1 << BTTN1_PIN);
    DDRB &= ~(1 << BTTN3_PIN);
    DDRC &= ~((1 << BTTN2_PIN) | (1 << BTTN4_PIN));

    // Enable pull-up resistors
    PORTD |= (1 << BTTN1_PIN);
    PORTB |= (1 << BTTN3_PIN);
    PORTC |= ((1 << BTTN2_PIN) | (1 << BTTN4_PIN));
}

uint8_t bttns_read() {
    bttns_states = 0;

    // Active LOW, so invert the bit logic
    if (!(PIND & (1 << BTTN1_PIN))) bttns_states |= (1 << BTTN1);
    if (!(PINC & (1 << BTTN2_PIN))) bttns_states |= (1 << BTTN2);
    if (!(PINB & (1 << BTTN3_PIN))) bttns_states |= (1 << BTTN3);
    if (!(PINC & (1 << BTTN4_PIN))) bttns_states |= (1 << BTTN4);

    return bttns_states;
}

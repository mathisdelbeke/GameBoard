#include "buttons.h"

#define BTTN1_PIN PD6
#define BTTN2_PIN PC3
#define BTTN3_PIN PB2
#define BTTN4_PIN PC0

static uint8_t bttns_states = 0;
static volatile uint8_t debounce_flag = 1; 

void bttns_init() {
    // Set pins as input
    DDRD &= ~(1 << BTTN1_PIN);
    DDRB &= ~(1 << BTTN3_PIN);
    DDRC &= ~((1 << BTTN2_PIN) | (1 << BTTN4_PIN));

    // Enable pull-up resistors
    PORTD |= (1 << BTTN1_PIN);
    PORTB |= (1 << BTTN3_PIN);
    PORTC |= ((1 << BTTN2_PIN) | (1 << BTTN4_PIN));

    init_debounce_timer();
}

void init_debounce_timer() {
    TCCR1B |= (1 << WGM12);                 // CTC mode
    TIMSK1 |= (1 << OCIE1A);                // Interrupt compare match
    TCCR1B |= (1 << CS11) | (1 << CS10);    // Prescaler 64
    OCR1A = 25000;                          
    TCNT1 = 0;                              // reset timer
}

uint8_t bttns_read() {
    if (!debounce_flag) {
        TCCR1B |= (1 << CS11) | (1 << CS10);    // Start debounce timer
        return 0;
    } 

    // Active LOW, so invert the bit logic
    bttns_states = 0;
    if (!(PIND & (1 << BTTN1_PIN))) bttns_states |= (1 << BTTN1);
    if (!(PINC & (1 << BTTN2_PIN))) bttns_states |= (1 << BTTN2);
    if (!(PINB & (1 << BTTN3_PIN))) bttns_states |= (1 << BTTN3);
    if (!(PINC & (1 << BTTN4_PIN))) bttns_states |= (1 << BTTN4);

    debounce_flag = 0;
    return bttns_states;
}

ISR(TIMER1_COMPA_vect) {
    debounce_flag = 1;      // Debounce time has passed
    TCCR1B &= ~((1 << CS11) | (1 << CS10)); // Stop debounce timer
    TCNT1 = 0;              // reset debounce timer
}

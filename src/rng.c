#include "rng.h"

#include "display.h"
#include <util/delay.h>

static unsigned int rng_state;

void rng_seed() {
    uint16_t seed = 0;
    ADMUX = (1 << REFS0);                               // AVcc ref  
    ADCSRA = (1 << ADEN) | (1 << ADPS1);                // Enable + prescale 4
    for (uint8_t i = 0; i < 3; i++) {                   // Take floating point samples from ADC0, ADC1, and ADC2
        ADMUX = (1 << REFS0) | i;                       
        _delay_ms(50);                                  // Wait for voltage on the pin and internal capacitor to settle
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));
        seed ^= ADC;                                    // Mix results
    }
    rng_state = seed;
}

// Simple LCG: Xn+1 = (a * Xn + c) % m
uint16_t rng_rand() {
    rng_state = (1103515245 * rng_state + 12345);
    return (rng_state >> 8) & 0x7FFF;  // 15-bit result
}

uint8_t rng_rand_range(uint8_t upper_limit) {
    return (rng_rand() % upper_limit); 
}

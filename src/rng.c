#include "rng.h"

static unsigned int rng_state = 1;

void rng_seed() {
    ADMUX = (1 << REFS0);                               // AVcc ref  
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS1);  // Enable + start ADC
    while (ADCSRA & (1 << ADSC));                       // Wait for complete
    rng_state = ADC;
}

// Simple LCG: Xn+1 = (a * Xn + c) % m
uint16_t rng_rand() {
    rng_state = (1103515245 * rng_state + 12345);
    return (rng_state >> 8) & 0x7FFF;  // 15-bit result
}

uint8_t rng_rand_range(uint8_t upper_limit) {
    return (rng_rand() % upper_limit); 
}

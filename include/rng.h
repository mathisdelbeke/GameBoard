#ifndef RNG_H
#define RNG_H

#include <avr/io.h>

void rng_seed();
uint16_t rng_rand();
uint8_t rng_rand_range(uint8_t upper_limit);

#endif

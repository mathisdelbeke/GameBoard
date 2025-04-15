#ifndef RNG_H
#define RNG_H

#include "space_invader.h"
#include <avr/io.h>

void rng_seed();
uint16_t rng_rand(void);
uint8_t rng_rand_range(void);

#endif

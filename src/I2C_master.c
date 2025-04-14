#include "I2C_master.h"

#define F_CPU 16000000UL
#define SCL_CLOCK 400000L  
// 100 kHz common Standard Mode

void i2c_init() {
    TWSR = 0x00;                            // Prescaler of 1 in Status Register
    TWBR = ((F_CPU/SCL_CLOCK) - 16) / 2;    // Two-Wire Bit Rate Register set for 100 kHz
    TWCR = (1 << TWEN);                     // Enable in Two Wire Control Register
}

void i2c_start(void) {
    TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT); // START condition bit, enable bit, clear interrupt flag
    while (!(TWCR & (1<<TWINT)));               // Wait for interrupt flag to be 1 (=done)
}

void i2c_write(uint8_t data) {
    TWDR = data;                    // Load data in Data Register
    TWCR = (1<<TWEN) | (1<<TWINT);  // Begin transmission, enable bit, clear interrupt flag
    while (!(TWCR & (1<<TWINT)));   // Wait for interrupt flag to be 1 (=done)
}

uint8_t i2c_read(uint8_t ack) {
    // If ack == 1, master sets TWEA and sends ACK, expects more data
    // If ack == 0, master clears TWEA sends NACK, stop reading
    TWCR = (1<<TWEN) | (1<<TWINT) | (ack ? (1<<TWEA) : 0);
    while (!(TWCR & (1<<TWINT)));   // Wait for interrupt flag to be 1 (=done)
    return TWDR;                    // Data Register holds received byte
}

void i2c_stop(void) {
    TWCR = (1<<TWSTO) | (1<<TWEN) | (1<<TWINT); // STOP condition bit, etc.
}

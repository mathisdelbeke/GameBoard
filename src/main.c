#include "buttons.h"
#include "uart.h"
#include "I2C_master.h"
#include "display.h"

uint8_t bttns_states;

int main() {
    bttns_init();
    uart_init(9600);
    i2c_init();
    oled_init();
    oled_fill(0x00);

    oled_set_cursor(0, 0);
    oled_write_string("Hello");

    bttns_states = bttns_read();
    for (int i = 0; i < 4; i++) {
        uart_print_unsigned((bttns_states & (1 << i)));
        uart_print_string("\r\n");
    }
    uart_print_string("\r\n");
    
    while(1) {

    }
}
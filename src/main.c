#include "buttons.h"
#include "uart.h"

uint8_t bttns_states;

int main() {
    bttns_init();
    uart_init(9600);

    bttns_states = bttns_read();
    for (int i = 0; i < 4; i++) {
        uart_print_unsigned((bttns_states & (1 << i)));
        uart_print_string("\r\n");
    }
    uart_print_string("\r\n");
    
    while(1) {

    }
}
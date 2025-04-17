#include "buttons.h"
#include "uart.h"
#include "display.h"
#include "gap_rush.h"
#include <util/delay.h>

#define SPACE_INVADER 0

static uint8_t selected_option = 0;
static uint8_t prev_selected_option = 0;
static uint8_t bttns_states;

void init_peripherals() {
    bttns_init();
    uart_init(9600);
    oled_init();
}

void show_menu() {
    oled_draw_cursor(prev_selected_option, selected_option);
    oled_set_cursor(0, 0);
    oled_write_string("Start Space Invader");
    oled_set_cursor(0, 1);
    oled_write_string("Other thing");
    oled_set_cursor(0, 2);
    oled_write_string("Other thing");
}

void update_menu() {
    bttns_states = bttns_read();
    if (bttns_states & (1 << BTTN1)) {
        prev_selected_option = selected_option;
        if (selected_option < 2) selected_option++;    
        else selected_option = 0;
        oled_draw_cursor(prev_selected_option, selected_option);
    }
    else if (bttns_states & (1 << BTTN2)) {
        if (selected_option == SPACE_INVADER) {
            play_gap_rush();
            oled_fill(0x00);        // If game is done
            show_menu();
        }
    }
}

int main() {
    init_peripherals();
    show_menu();

    while(1) {
        update_menu();
    }
}
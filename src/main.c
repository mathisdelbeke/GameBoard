#include "buttons.h"
#include "uart.h"
#include "I2C_master.h"
#include "display.h"
#include <util/delay.h>

#define BTTN1 0
#define BTTN2 1
#define BTTN3 2
#define BTTN4 3

static const uint8_t cursor[] = {0x08,0x1C,0x22,0x08,0x08}; // "-----"

static uint8_t selected_option = 0;
static uint8_t prev_selected_option = 0;
static uint8_t bttns_states;

void draw_cursor() {
    // Clear cursor on old place
    oled_set_cursor((SCREEN_WIDTH - 10), prev_selected_option);
    for (uint8_t i = 0; i < sizeof(cursor); i++) {
        oled_send_data(0x00);
    }

    // Draw cursor on new place
    oled_set_cursor((SCREEN_WIDTH - 10), selected_option);
    for (uint8_t i = 0; i < sizeof(cursor); i++) {
        oled_send_data(cursor[i]);
    }

    prev_selected_option = selected_option;
}

void show_menu() {
    draw_cursor();
    oled_set_cursor(0, 0);
    oled_write_string("Start Tetris");
    oled_set_cursor(0, 1);
    oled_write_string("Other thing");
    oled_set_cursor(0, 2);
    oled_write_string("Other thing");
}

void update_menu() {
    if (bttns_states & (1 << BTTN1)) {
        if (selected_option < 2) selected_option++;    // magic number
        else selected_option = 0;
        show_menu();
    }
    else if (bttns_states & (1 << BTTN2)) oled_fill(0x00);
}

int main() {
    bttns_init();
    uart_init(9600);
    i2c_init();
    oled_init();
    oled_fill(0x00);
    show_menu();

    while(1) {
        bttns_states = bttns_read();
        update_menu();
        _delay_ms(100);
    }
}
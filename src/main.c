#include "buttons.h"
#include "uart.h"
#include "display.h"
#include "gap_rush.h"
#include "memory_test.h"
#include "snake.h"
#include "pathfinder.h"

typedef enum {
    GAP_RUSH,
    MEMORY_TEST,
    SNAKE,
    PATHFINDER,
    NUM_GAMES                   // Auto increase when adding games to enum
} GameID;

static const uint8_t INDICATOR[] = {0x08,0x1C,0x22,0x08,0x08}; 
static const uint8_t INDICATOR_POS_X = (SCREEN_WIDTH - 10);

static uint8_t selected_option = 0;
static uint8_t prev_selected_option = 0;

static void (*timer_hit_callback)(void);

static void init_peripherals() {
    bttns_init();
    uart_init(9600);
    oled_init();
}

static void erase_indicator() {
    oled_set_cursor(INDICATOR_POS_X, prev_selected_option);                    
    for (uint8_t i = 0; i < sizeof(INDICATOR); i++) { 
        oled_send_data(0x00);                           // Clear pixel
    }
}

static void draw_indicator() {
    erase_indicator();
    oled_set_cursor(INDICATOR_POS_X, selected_option);
    for (uint8_t i = 0; i < sizeof(INDICATOR); i++) {
        oled_send_data(INDICATOR[i]);
    }
}

static void show_menu() {
    draw_indicator();
    oled_set_cursor(0, 0);
    oled_write_string("Start Gap Rush");
    oled_set_cursor(0, 1);
    oled_write_string("Start Memory Test");
    oled_set_cursor(0, 2);
    oled_write_string("Start Snake");
    oled_set_cursor(0, 3);
    oled_write_string("Start Pathfinder");
}

static void update_menu() {
    uint8_t bttns_states = bttns_read();
    if (bttns_states & (1 << BTTN1)) {
        prev_selected_option = selected_option;
        if (selected_option < (NUM_GAMES - 1)) selected_option++;    
        else selected_option = 0;
        draw_indicator();
    }
    else if (bttns_states & (1 << BTTN2)) {
        if (selected_option == GAP_RUSH) {
            timer_hit_callback = gap_rush_timer_hit;
            play_gap_rush();
            oled_fill(0x00);        // If game is done
            show_menu();
        }
        else if (selected_option == MEMORY_TEST) {
            play_memory_test();
            oled_fill(0x00);        // If game is done
            show_menu();
        }
        else if (selected_option == SNAKE) {
            timer_hit_callback = snake_timer_hit;
            play_snake();
            oled_fill(0x00);        // If game is done
            show_menu();
        }
        else if (selected_option == PATHFINDER) {
            play_pathfinder();
            oled_fill(0X00);        // If game is done
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

ISR(TIMER0_COMPA_vect) {
    if (timer_hit_callback) timer_hit_callback();
}
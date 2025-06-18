#include "memory_test.h"

#define ARROW_PIXEL_VALUES 16

static const uint8_t ARROWS_HORIZONTAL[][ARROW_PIXEL_VALUES] = {{0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF},
                                                                    {0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01}};

static const uint8_t ARROWS_VERITCAL[][ARROW_PIXEL_VALUES] = {{0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80},
                                                                  {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01}};

static void init_game();
static void draw_horizontal_arrow(uint8_t idx);
static void draw_vertical_arrow(uint8_t idx);

void play_memory_test() {
    init_game();
    uint8_t game_active = 1;
    while(game_active) {
        draw_horizontal_arrow(DIRECTION_LEFT);
        _delay_ms(1000);
        draw_horizontal_arrow(DIRECTION_RIGHT);
        _delay_ms(1000);
        draw_vertical_arrow(DIRECTION_UP);
        _delay_ms(1000);
        draw_vertical_arrow(DIRECTION_DOWN);
        _delay_ms(1000);
    }
}

static void init_game() {
    oled_fill(0x00);
}

static void draw_horizontal_arrow(uint8_t idx) {
    oled_fill(0x00);
    oled_set_cursor((SCREEN_WIDTH / 2), ((SCREEN_PAGES / 2) - 1));
    for(uint8_t i = 0; i < ARROW_PIXEL_VALUES / 2; i++) {
        oled_send_data(ARROWS_HORIZONTAL[idx][i]);
    }
    oled_set_cursor((SCREEN_WIDTH / 2), (SCREEN_PAGES / 2));
    for(uint8_t i = ARROW_PIXEL_VALUES / 2; i < ARROW_PIXEL_VALUES; i++) {
        oled_send_data(ARROWS_HORIZONTAL[idx][i]);
    }
}

static void draw_vertical_arrow(uint8_t idx) {
    oled_fill(0x00);
    oled_set_cursor(((SCREEN_WIDTH / 2) - (ARROW_PIXEL_VALUES / 2)), (SCREEN_PAGES / 2));
    for(uint8_t i = 0; i < ARROW_PIXEL_VALUES; i++) {
        oled_send_data(ARROWS_VERITCAL[idx][i]);
    }
}
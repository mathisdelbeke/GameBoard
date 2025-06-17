#ifndef DISPLAY_H
#define DISPLAY_H

#include "I2C_master.h"

#define SCREEN_WIDTH 128
#define SCREEN_PAGES 8

void oled_init();
void oled_send_data(uint8_t data);
void oled_fill(uint8_t color);
void oled_set_cursor(uint8_t x, uint8_t page);
void oled_write_char(char c);
void oled_write_string(const char* str);

#endif
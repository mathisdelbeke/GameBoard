#ifndef DISPLAY_H
#define DISPLAY_H

#include <avr/io.h>

void oled_command(uint8_t cmd);
void oled_send_data(uint8_t data);
void oled_init();
void oled_set_cursor(uint8_t x, uint8_t page);
void oled_write_char(char c);
void oled_write_string(const char* str);
void oled_fill(uint8_t color);

#endif
#ifndef DISPLAY_H
#define DISPLAY_H

#define SCREEN_WIDTH 128
#define SCREEN_PAGES 8

#include "I2C_master.h"
#include "gap_rush.h"
#include <util/delay.h>
#include <avr/io.h>

void oled_command(uint8_t cmd);
void oled_send_data(uint8_t data);
void oled_init();
void oled_fill(uint8_t color);
void oled_set_cursor(uint8_t x, uint8_t page);

void oled_write_char(char c);
void oled_write_string(const char* str);

void oled_draw_user(User user);
void oled_erase_user(Pos user_pos);
void oled_draw_rock(Rock rock);
void oled_erase_rock(Rock rock);

#endif
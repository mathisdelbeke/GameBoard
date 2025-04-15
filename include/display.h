#ifndef DISPLAY_H
#define DISPLAY_H

#define SCREEN_WIDTH 128

#include "I2C_master.h"
#include "space_invader.h"
#include <util/delay.h>
#include <avr/io.h>

void oled_command(uint8_t cmd);
void oled_send_data(uint8_t data);
void oled_init();
void oled_fill(uint8_t color);
void oled_set_cursor(uint8_t x, uint8_t page);

void oled_write_char(char c);
void oled_write_string(const char* str);

void oled_draw_cursor(uint8_t prev_page, uint8_t page);
void oled_erase_cursor(uint8_t prev_page);
void oled_draw_user(User user);
void oled_erase_user(Pos user_pos);
void draw_rocks(Rock rocks[]);

#endif
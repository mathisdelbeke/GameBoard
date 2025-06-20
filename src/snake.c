#include "snake.h"

#include <stdlib.h>

#define COLUMNS_PER_SNAKE_BLOCK 8                                                                                       // 8 x 8 pixels per snake block

typedef enum {
    DIRECTION_RIGHT = 0,
    DIRECTION_LEFT = 1,
    DIRECTION_UP = 2,
    DIRECTION_DOWN = 3
}Direction;

typedef struct {
    uint8_t x;
    uint8_t y;
}Pos;

typedef struct {
    Pos pos;
    Direction direction;
}Snake;

static Snake snake = {.pos = {.x = 0, .y = 0}, .direction = DIRECTION_UP};

static uint8_t game_active = 1;
static volatile uint8_t timer_hit_count = 0;
static volatile uint8_t timer_hits_till_move = 31;
static volatile uint8_t time_to_move = 0;

static void init_game();
static void init_move_timer();
static void draw_snake();
static void move_snake();
static void read_user_input();

void play_snake() {
    init_game();
    while (game_active) {
        read_user_input();
        if (time_to_move) {
            move_snake();
            draw_snake();
        }
    }
    TCCR0B &= ~((1 << CS02) | (1 << CS00));     // Turn timer off
}

static void init_game() {
    oled_fill(0x00);
    snake.pos.x = 0;
    snake.pos.y = 0;
    snake.direction = DIRECTION_RIGHT;
    timer_hit_count = 0;
    timer_hits_till_move = 31;
    game_active = 1;
    init_move_timer();
}

static void init_move_timer() {
    TCCR0A |= (1 << WGM01);                     // CTC mode
    TCCR0B |= (1 << CS02) | (1 << CS00);        // Prescaler 1024
    OCR0A = 249;                                // Compare value for 16ms
    TIMSK0 |= (1 << OCIE0A);                    // Enable Timer0 Compare Match A interrupt
}

static void draw_snake() {
    oled_fill(0x00);
    oled_set_cursor((snake.pos.x), (snake.pos.y));
    for (uint8_t i = 0; i < COLUMNS_PER_SNAKE_BLOCK; i++) {
        oled_send_data(0xFF);
    }
}

static void read_user_input() {
    uint8_t bttns_states = bttns_read();
    if (bttns_states & (1 << BTTN1)) {
        if (snake.direction != DIRECTION_RIGHT) snake.direction = DIRECTION_LEFT;
    }
    else if (bttns_states & (1 << BTTN2)) {
        if (snake.direction != DIRECTION_LEFT) snake.direction = DIRECTION_RIGHT;
    }
    else if (bttns_states & (1 << BTTN3)) {
        if (snake.direction != DIRECTION_DOWN) snake.direction = DIRECTION_UP;
    }
    else if (bttns_states & (1 << BTTN4)) {
        if (snake.direction != DIRECTION_UP) snake.direction = DIRECTION_DOWN;
    }
}

static void move_snake() {
    if (snake.direction == DIRECTION_RIGHT) {
        if (snake.pos.x < (SCREEN_WIDTH - COLUMNS_PER_SNAKE_BLOCK)) snake.pos.x += COLUMNS_PER_SNAKE_BLOCK;
        else snake.pos.x = 0;
    }
    else if (snake.direction == DIRECTION_LEFT) {
        if (snake.pos.x > 0) snake.pos.x -= COLUMNS_PER_SNAKE_BLOCK;
        else snake.pos.x = (SCREEN_WIDTH - COLUMNS_PER_SNAKE_BLOCK);
    }
    else if (snake.direction == DIRECTION_UP) {
        if (snake.pos.y > 0) snake.pos.y -= 1;
        else snake.pos.y = (SCREEN_PAGES - 1);
    }
    else if (snake.direction == DIRECTION_DOWN) {
        if (snake.pos.y < (SCREEN_PAGES - 1)) snake.pos.y += 1;
        else snake.pos.y = 0;
    }
    time_to_move = 0;
}

void snake_timer_hit() {
    timer_hit_count++;
    if (timer_hit_count >= timer_hits_till_move) {    // Move delay = 16 ms * timer_hits_till_move
        timer_hit_count = 0;
        time_to_move = 1;
    }
}
#include "snake.h"

#include <stdlib.h>

#define COLUMNS_PER_SNAKE_BLOCK 8       // 8 x 8 pixels per snake block
#define COLUMNS_PER_FOOD_BLOCK 8        // 8 x 8 pixels per food block, but only 4 x 4 will be visual
#define FOOD_VISUAL_OFFSET 2            // Draw food smaller to differ from snake

#define INITIAL_HITS_TILL_MOVE 31
#define MAX_SNAKE_LENGTH 128

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
    Pos block_positions[MAX_SNAKE_LENGTH];
    Direction direction;
    uint8_t length;
}Snake;

static uint8_t game_active = 1;
static volatile uint8_t timer_hit_count = 0;
static volatile uint8_t timer_hits_till_move = INITIAL_HITS_TILL_MOVE;
static volatile uint8_t time_to_move = 0;

static Snake snake = {.direction = DIRECTION_UP, .length = 1};
static Pos food_pos;    
static Pos prev_tail_pos;
static Direction current_direction_change;                                      

static void init_game();
static void init_move_timer();
static void process_user_input();
static void update_snake_direction();
static void place_food();
static void move_snake();
static void check_food_collision();
static void check_snake_collision();
static void draw_snake();
static void draw_food();

void play_snake() {
    init_game();
    while (game_active) {
        process_user_input();
        if (time_to_move) {
            update_snake_direction();
            move_snake();
            check_food_collision();
            check_snake_collision();
            oled_fill(0x00);
            draw_snake();
            draw_food();
            time_to_move = 0;
        }
    }
    TCCR0B &= ~((1 << CS02) | (1 << CS00));     // Turn timer off
}

static void init_game() {
    rng_seed();
    oled_fill(0x00);
    snake.block_positions[0].x = 0;
    snake.block_positions[0].y = 0;
    snake.direction = DIRECTION_RIGHT;
    snake.length = 1;
    timer_hit_count = 0;
    timer_hits_till_move = INITIAL_HITS_TILL_MOVE;
    game_active = 1;
    place_food();
    init_move_timer();
}

static void init_move_timer() {
    TCCR0A |= (1 << WGM01);                     // CTC mode
    TCCR0B |= (1 << CS02) | (1 << CS00);        // Prescaler 1024
    OCR0A = 249;                                // Compare value for 16ms
    TIMSK0 |= (1 << OCIE0A);                    // Enable Timer0 Compare Match A interrupt
}

static void process_user_input() {
    uint8_t bttns_states = bttns_read();
    if (bttns_states & (1 << BTTN1)) current_direction_change = DIRECTION_LEFT;
    else if (bttns_states & (1 << BTTN2)) current_direction_change = DIRECTION_RIGHT;
    else if (bttns_states & (1 << BTTN3)) current_direction_change = DIRECTION_UP;
    else if (bttns_states & (1 << BTTN4)) current_direction_change = DIRECTION_DOWN;
}

static void update_snake_direction() {
    if (current_direction_change == DIRECTION_LEFT) {
        if (snake.direction != DIRECTION_RIGHT) snake.direction = DIRECTION_LEFT;       // To opposite direction isn't allowed
    }
    else if (current_direction_change == DIRECTION_RIGHT) {
        if (snake.direction != DIRECTION_LEFT) snake.direction = DIRECTION_RIGHT;
    }
    else if (current_direction_change == DIRECTION_UP) {
        if (snake.direction != DIRECTION_DOWN) snake.direction = DIRECTION_UP;
    }
    else if (current_direction_change == DIRECTION_DOWN) {
        if (snake.direction != DIRECTION_UP) snake.direction = DIRECTION_DOWN;
    }
}

static void place_food() {
    uint8_t random_x = rng_rand_range((SCREEN_WIDTH - COLUMNS_PER_FOOD_BLOCK));
    food_pos.x = ((random_x / COLUMNS_PER_FOOD_BLOCK) * COLUMNS_PER_FOOD_BLOCK);        // Use closest multiplicity
    food_pos.y = rng_rand_range(SCREEN_PAGES - 1);
}

static void move_snake() {
    prev_tail_pos = snake.block_positions[snake.length - 1];                            // Keep track for growing when food collision
    
    for (uint8_t i = (snake.length - 1); i > 0; i--) {                                  // Shift all but head 
        snake.block_positions[i] = snake.block_positions[i - 1];
    }
    
    if (snake.direction == DIRECTION_RIGHT) {
        if (snake.block_positions[0].x < (SCREEN_WIDTH - COLUMNS_PER_SNAKE_BLOCK)) snake.block_positions[0].x += COLUMNS_PER_SNAKE_BLOCK;
        else snake.block_positions[0].x = 0;
    }
    else if (snake.direction == DIRECTION_LEFT) {
        if (snake.block_positions[0].x > 0) snake.block_positions[0].x -= COLUMNS_PER_SNAKE_BLOCK;
        else snake.block_positions[0].x = (SCREEN_WIDTH - COLUMNS_PER_SNAKE_BLOCK);
    }
    else if (snake.direction == DIRECTION_UP) {
        if (snake.block_positions[0].y > 0) snake.block_positions[0].y -= 1;
        else snake.block_positions[0].y = (SCREEN_PAGES - 1);
    }
    else if (snake.direction == DIRECTION_DOWN) {
        if (snake.block_positions[0].y < (SCREEN_PAGES - 1)) snake.block_positions[0].y += 1;
        else snake.block_positions[0].y = 0;
    }
}

static void check_food_collision() {
    if ((snake.block_positions[0].x == food_pos.x) && (snake.block_positions[0].y == food_pos.y)) {
        snake.length++;
        snake.block_positions[snake.length - 1] = prev_tail_pos;
        place_food();
    }
}

static void check_snake_collision() {
                                        // Only self collision possible with length of 4
        for (uint8_t i = 1; i < snake.length; i++) {
            Pos snake_block = snake.block_positions[i];
            if ((snake.block_positions[0].x == snake_block.x) && (snake.block_positions[0].y == snake_block.y)) {
                game_active = 0;
            }
        }
    
}

static void draw_snake() {
    for (uint8_t i = 0; i < snake.length; i++)  {
        oled_set_cursor((snake.block_positions[i].x), (snake.block_positions[i].y));
        for (uint8_t i = 0; i < COLUMNS_PER_SNAKE_BLOCK; i++) {
            oled_send_data(0xFF);
        }
    }
}

static void draw_food() {
    oled_set_cursor((food_pos.x + FOOD_VISUAL_OFFSET), food_pos.y);
    for (uint8_t i = 0; i < (COLUMNS_PER_FOOD_BLOCK - (FOOD_VISUAL_OFFSET * 2)); i++) {
        oled_send_data(0x3C);
    }    
}

// ISR in main will call this callback
void snake_timer_hit() {
    timer_hit_count++;
    if (timer_hit_count >= timer_hits_till_move) {                                  // Move delay = 16 ms * timer_hits_till_move
        timer_hit_count = 0;
        time_to_move = 1;
    }
}

/* 
Todo:   little pixel diff food
        snake collision
        read two movements at once
        draw effecient, only head tail?
*/
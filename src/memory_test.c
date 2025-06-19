#include "memory_test.h"

#define MAX_SEQUENCE_LENGTH 5
#define NO_INPUT 255
#define ARROW_PIXEL_VALUES 16

typedef enum {
    AXIS_HORIZONTAL = 0,
    AXIS_VERTICAL = 1,
} Axis;

typedef enum {
    DIRECTION_LEFT = 0,
    DIRECTION_RIGHT = 1,
    DIRECTION_UP = 0,
    DIRECTION_DOWN = 1
} Direction;

typedef struct {
    uint8_t axis_idx : 1;                                   // Uses only 1 bit for less waste
    uint8_t direction_idx : 1;
    uint8_t reserve_bits : 6;
} Arrow;

static const uint8_t ARROWS_HORIZONTAL[][ARROW_PIXEL_VALUES] = {{0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF},          
                                                                    {0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01}};

static const uint8_t ARROWS_VERITCAL[][ARROW_PIXEL_VALUES] = {{0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80},
                                                                  {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01}};

static uint8_t game_active = 1;

static Arrow arrow_sequence[MAX_SEQUENCE_LENGTH];
static uint8_t arrow_idx = 0;

static void init_game();
static void add_arrow_to_sequence();
static void show_arrow_sequence();
static void show_user_input_screen();
static void read_all_user_inputs();
static void read_user_input(uint8_t *user_axis_input, uint8_t *user_direction_input);
static void check_user_input(uint8_t *user_axis_input, uint8_t *user_direction_input, uint8_t *num_user_inputs, uint8_t *reading);
static void check_max_sequence_reached();
static void draw_horizontal_arrow(Direction direction);
static void draw_vertical_arrow(Direction direction);

void play_memory_test() {
    init_game();
    while(game_active) {
        add_arrow_to_sequence();
        show_arrow_sequence();
        show_user_input_screen();
        read_all_user_inputs();
        check_max_sequence_reached();
    }
}

static void init_game() {
    oled_fill(0x00);
    rng_seed();
    arrow_idx = 0;
    game_active = 1;
}

static void add_arrow_to_sequence() {
    uint8_t random_axis_idx = rng_rand_range(2);                                                
    uint8_t random_direction_idx = rng_rand_range(2);
    Arrow random_arrow = {.axis_idx = random_axis_idx, .direction_idx = random_direction_idx};
    
    arrow_sequence[arrow_idx] = random_arrow;                                     
}

static void show_arrow_sequence() {
    for (uint8_t i = 0; i <= arrow_idx; i++) {
        Arrow arrow = arrow_sequence[i];

        if (arrow.axis_idx == AXIS_HORIZONTAL) {
            if (arrow.direction_idx == DIRECTION_LEFT) 
                draw_horizontal_arrow(DIRECTION_LEFT);
            else if (arrow.direction_idx == DIRECTION_RIGHT)
                draw_horizontal_arrow(DIRECTION_RIGHT);
            _delay_ms(1000);                                                            
        }

        else if (arrow.axis_idx == AXIS_VERTICAL) {
            if (arrow.direction_idx == DIRECTION_UP) 
                draw_vertical_arrow(DIRECTION_UP);
            else if (arrow.direction_idx == DIRECTION_DOWN)
                draw_vertical_arrow(DIRECTION_DOWN);
            _delay_ms(1000);
        }
    }
}

static void show_user_input_screen() {
    oled_fill(0x00);
    oled_set_cursor(0, 0);
    oled_write_string("Repeat sequence now");
}

static void read_all_user_inputs() {
    uint8_t num_user_inputs = 0;
    uint8_t reading = 1;
    while (reading) {              
        uint8_t user_axis_input = NO_INPUT;
        uint8_t user_direction_input = NO_INPUT;      
        read_user_input(&user_axis_input, &user_direction_input);
        check_user_input(&user_axis_input, &user_direction_input, &num_user_inputs, &reading);
    }
}

static void read_user_input(uint8_t *user_axis_input, uint8_t *user_direction_input) {
    while ((*user_axis_input == NO_INPUT) || (*user_direction_input == NO_INPUT)) {
        uint8_t bttns_states = bttns_read();
        if (bttns_states & (1 << BTTN1)) {
            *user_axis_input = 0;
            *user_direction_input = 0;
        }
        else if (bttns_states & (1 << BTTN2)) {
            *user_axis_input = 0;
            *user_direction_input = 1;
        }
        else if (bttns_states & (1 << BTTN3)) {
            *user_axis_input = 1;
            *user_direction_input = 0;
        }
        else if (bttns_states & (1 << BTTN4)) {
            *user_axis_input = 1;
            *user_direction_input = 1;
        }
    }
}

static void check_user_input(uint8_t *user_axis_input, uint8_t *user_direction_input, uint8_t *num_user_inputs, uint8_t *reading) {
    Arrow correct_arrow = arrow_sequence[*num_user_inputs];
    if ((correct_arrow.axis_idx == *user_axis_input) && (correct_arrow.direction_idx == *user_direction_input)) {
        if (*num_user_inputs == arrow_idx) *reading = 0;                                                                                    // If full sequence repeated, stop reading
        else                                                                                                                                // Else keep reading
            (*num_user_inputs)++;
    }
    else {                                                                                                                                  // Mistake, end game
        *reading = 0;
        game_active = 0;
    }
}

static void check_max_sequence_reached() {
    if (arrow_idx < (MAX_SEQUENCE_LENGTH - 1)) arrow_idx++;                                                                                 // Stop game if max sequence is reached
    else {
        oled_fill(0x00);
        oled_set_cursor(0,0);
        oled_write_string("Max sequence reached");
        _delay_ms(3000);
        game_active = 0;
    }
}

static void draw_horizontal_arrow(Direction direction) {
    oled_fill(0x00);
    oled_set_cursor((SCREEN_WIDTH / 2), ((SCREEN_PAGES / 2) - 1));
    for(uint8_t i = 0; i < ARROW_PIXEL_VALUES / 2; i++) {
        oled_send_data(ARROWS_HORIZONTAL[direction][i]);
    }
    oled_set_cursor((SCREEN_WIDTH / 2), (SCREEN_PAGES / 2));
    for(uint8_t i = ARROW_PIXEL_VALUES / 2; i < ARROW_PIXEL_VALUES; i++) {
        oled_send_data(ARROWS_HORIZONTAL[direction][i]);
    }
}

static void draw_vertical_arrow(Direction direction) {
    oled_fill(0x00);
    oled_set_cursor(((SCREEN_WIDTH / 2) - (ARROW_PIXEL_VALUES / 2)), (SCREEN_PAGES / 2));
    for(uint8_t i = 0; i < ARROW_PIXEL_VALUES; i++) {
        oled_send_data(ARROWS_VERITCAL[direction][i]);
    }
}
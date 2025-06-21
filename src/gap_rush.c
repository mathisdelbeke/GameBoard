#include "gap_rush.h"

#define USER_WIDTH 8
#define ROCK_WIDTH 128         
#define ROCK_HOLE_SIZE USER_WIDTH                                          //((USER_WIDTH * 2) + 1)

typedef struct {
    uint8_t x;
    uint8_t y;
} Pos;

typedef struct {
    uint8_t shape[USER_WIDTH];
    Pos old_pos;
    Pos pos;
} User;

typedef struct {
    uint8_t hole_x;
    Pos old_pos;
    Pos pos;
} Rock;

// Makes (8 x 8 pixels) shape on the bottom of screen
static User user = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, .old_pos = {0, (SCREEN_PAGES - 1)}, .pos = {0, (SCREEN_PAGES - 1)}};
static Rock rock;

static uint8_t game_active = 0;
static uint8_t bttns_states = 0;                
static uint8_t lines_cleared = 0;
static uint8_t rock_drop_delay;                         // Decreases throughout levels
static const uint8_t lines_per_level = 3;               // Lines to clear before increase level

static volatile uint8_t timer_hit_count = 0;            // Timer is too fast, count ticks to 'rock_drop_delay' before rendering
static volatile uint8_t time_to_update_rock = 0;        // When 'timer_hit_count' = 'rock_drop_delay', time to rerender rock

static void init_game();
static void generate_rock();
static void update_user();
static void update_rock();
static void check_collision();
static void update_level();
static void draw_user();
static void erase_user();
static void draw_rock();
static void erase_rock();

void play_gap_rush() {
    init_game();
    while (game_active) {
        update_user();                          // Reads buttons and rerenders as fast as possible
        if (time_to_update_rock) update_rock();      // If time to render is hit, rerender and check rock
    }
    TCCR0B &= ~((1 << CS02) | (1 << CS00));     // Turn timer off
}

static void init_game() {
    oled_fill(0x00);                            // Clear oled
    draw_user();
    rng_seed();                                 // Seed the random generator
    generate_rock();
    lines_cleared = 0;
    rock_drop_delay = 20;                       // Initial rock_drop_delay
    game_active = 1;

    TCCR0A |= (1 << WGM01);                     // CTC mode
    TCCR0B |= (1 << CS02) | (1 << CS00);        // Prescaler 1024
    OCR0A = 249;                                // Compare value for 16ms
    TIMSK0 |= (1 << OCIE0A);                    // Enable Timer0 Compare Match A interrupt
}

void generate_rock() {
    uint8_t random_x = rng_rand_range((SCREEN_WIDTH - ROCK_HOLE_SIZE));
    random_x = (random_x / ROCK_HOLE_SIZE) * ROCK_HOLE_SIZE;               // Closest multiplication
    rock = (Rock){ .hole_x = random_x, .old_pos = {0,0}, .pos = {0,0} };   // Rock at the top of screen, with hole in it
    draw_rock();
} 

void update_user() {
    bttns_states = bttns_read();            
    if (bttns_states & (1 << BTTN1)) {
        user.old_pos = user.pos;
        if (user.pos.x <= 0) user.pos.x = 0;        // Make sure user stays on screen
        else user.pos.x -= USER_WIDTH;              // Else move to left, 1 width at a time
        draw_user();
    }
    else if (bttns_states & (1 << BTTN2)) {
        user.old_pos = user.pos;
        if (user.pos.x >= SCREEN_WIDTH - USER_WIDTH) user.pos.x = SCREEN_WIDTH - USER_WIDTH; // Make sure user stays on screen
        else user.pos.x += USER_WIDTH;                                                       // Else move to right
        draw_user();
    }
}

void update_rock() {
    if (rock.pos.y == SCREEN_PAGES) {               // If rock at bottom
        check_collision();          
        erase_rock();
        generate_rock();                            // Make new rock
        update_level();                             
    }
    else {
        rock.old_pos = rock.pos;
        rock.pos.y++;                               // Move rock to bottom
        draw_rock();
    }
    time_to_update_rock = 0;                             // Reset time to render
}

void check_collision() {
    if ((user.pos.x < rock.hole_x) || ((user.pos.x + USER_WIDTH) > (rock.hole_x + ROCK_HOLE_SIZE))) {
        char lines_cleared_buffer[4];                                       // Max 3 digits + null terminator
        oled_fill(0x00);                                                    // Clear oled
        oled_set_cursor(0, 0);
        oled_write_string("Lines cleared: ");
        oled_set_cursor(0, 1);
        oled_write_string(itoa(lines_cleared, lines_cleared_buffer, 10));   // Print lines cleared as string
        _delay_ms(2000);
        game_active = 0;                                                    // End game, game over
    }
}

void update_level() {
    lines_cleared++;    
    if (lines_cleared % lines_per_level == 0) {     // If lines per level are cleared, increase difficulty
        rock_drop_delay--;
    }
}

void gap_rush_timer_hit() {
    timer_hit_count++;
    if (timer_hit_count >= rock_drop_delay) {    // Count to rock_drop_delay before rerendering rock 
        timer_hit_count = 0;
        time_to_update_rock = 1;
    }
}

void draw_user() {
    erase_user();                                       // Erase old pixels first
    oled_set_cursor(user.pos.x, user.pos.y);
    for (uint8_t i = 0; i < USER_WIDTH; i++) {          // Draw the shape of user
        oled_send_data(user.shape[i]);
    }
}

void erase_user() {
    oled_set_cursor(user.old_pos.x, user.old_pos.y);
    for (uint8_t i = 0; i < USER_WIDTH; i++) {
        oled_send_data(0x00);
    } 
}

void draw_rock() {
    erase_rock();
    oled_set_cursor(rock.pos.x, rock.pos.y);
    for (uint8_t i = 0; i < rock.hole_x; i++) {                             // Draw 8 pixels vertical until the hole is met
        oled_send_data(0xFF);
    }
    oled_set_cursor((rock.hole_x + ROCK_HOLE_SIZE), rock.pos.y);            // Restart after the hole, so user isn't erased
    for (uint8_t i = (rock.hole_x + ROCK_HOLE_SIZE); i < ROCK_WIDTH; i++) {
        oled_send_data(0xFF);
    }
}

void erase_rock() {
    oled_set_cursor(rock.old_pos.x, rock.old_pos.y);
    for (uint8_t i = 0; i < rock.hole_x; i++) {                             // Clear 8 pixels vertical until the hole is met
        oled_send_data(0x00);
    }

    oled_set_cursor((rock.hole_x + ROCK_HOLE_SIZE), rock.old_pos.y);
    for (uint8_t i = (rock.hole_x + ROCK_HOLE_SIZE); i < ROCK_WIDTH; i++) { // Restart after the hole, so user isn't erased
        oled_send_data(0x00);
    }
}

// Just in time, seems like hit
// No collision anymore after rock on bottom
#include "gap_rush.h"
#include "display.h"
#include "buttons.h"
#include "rng.h"

// Makes (8 x 8 pixels) shape on the bottom of screen
static User user = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, .old_pos = {0, (SCREEN_PAGES - 1)}, .pos = {0, (SCREEN_PAGES - 1)}};
static Rock rock;

static uint8_t game_active = 0;
static uint8_t bttns_states = 0;                
static uint8_t lines_cleared = 0;
static uint8_t rock_drop_delay;                 // Decreases throughout levels
static const uint8_t lines_per_level = 3;       // Lines to clear before increase level

static volatile uint8_t render_tick_count = 0;  // Timer is too fast, count ticks to 'rock_drop_delay' before rendering
static volatile uint8_t rock_timer_hit = 0;     // When 'render_tick_count' = 'rock_drop_delay', time to rerender rock

void play_gap_rush() {
    init_game();
    game_active = 1;
    while (game_active) {
        update_user();                          // Reads buttons and rerenders as fast as possible
        if (rock_timer_hit) update_rock();      // If time to render is hit, rerender and check rock
    }
}

void init_game() {
    oled_fill(0x00);                            // Clear oled
    oled_draw_user(user);
    rng_seed();                                 // Seed the random generator
    generate_rock();
    lines_cleared = 0;
    rock_drop_delay = 20;                       // Initial rock_drop_delay

    TCCR0A |= (1 << WGM01);                     // CTC mode
    TCCR0B |= (1 << CS02) | (1 << CS00);        // Prescaler 1024
    OCR0A = 249;                                // Compare value for 16ms
    TIMSK0 |= (1 << OCIE0A);                    // Enable Timer0 Compare Match A interrupt
}

void generate_rock() {
    uint8_t random_x1 = rng_rand_range((SCREEN_WIDTH - ROCK_HOLE_SIZE));       
    rock = (Rock){ .hole_x = random_x1, .old_pos = {0,0}, .pos = {0,0} };   // Rock at the top of screen, with hole in it
    oled_draw_rock(rock);
}

void update_user() {
    bttns_states = bttns_read();            
    if (bttns_states & (1 << BTTN1)) {
        user.old_pos = user.pos;
        if (user.pos.x <= 0) user.pos.x = 0;        // Make sure user stays on screen
        else user.pos.x -= USER_WIDTH;              // Else move to left, 1 width at a time
        oled_draw_user(user);
    }
    else if (bttns_states & (1 << BTTN2)) {
        user.old_pos = user.pos;
        if (user.pos.x >= SCREEN_WIDTH - USER_WIDTH) user.pos.x = SCREEN_WIDTH - USER_WIDTH; // Make sure user stays on screen
        else user.pos.x += USER_WIDTH;                                                       // Else move to right
        oled_draw_user(user);
    }
    else if (bttns_states & (1 << BTTN3)) {         // Exit game before ending
        game_active = 0;
    }
}

void update_rock() {
    if (rock.pos.y == SCREEN_PAGES) {               // If rock at bottom
        check_collision();          
        oled_erase_rock(rock);
        generate_rock();                            // Make new rock
        update_level();                             
    }
    else {
        rock.old_pos = rock.pos;
        rock.pos.y++;                               // Move rock to bottom
        oled_draw_rock(rock);
    }
    rock_timer_hit = 0;                             // Reset time to render
}

void update_level() {
    lines_cleared++;    
    if (lines_cleared % lines_per_level == 0) {     // If lines per level are cleared, increase difficulty
        rock_drop_delay--;
    }
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

ISR(TIMER0_COMPA_vect) {                    
    render_tick_count++;
    if (render_tick_count >= rock_drop_delay) {    // Count to rock_drop_delay before rerendering rock 
        render_tick_count = 0;
        rock_timer_hit = 1;
    }
}

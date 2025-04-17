#include "space_invader.h"
#include "display.h"
#include "buttons.h"
#include "rng.h"

#include "uart.h"

static User user = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, .old_pos = {0, 7}, .pos = {0, 7}};
static Rock rock;

static uint8_t game_active = 0;
static uint8_t bttns_states = 0;
static uint8_t lines_cleared = 0;
static uint8_t slowness = 20; 
static const uint8_t lines_per_level = 3;
static char lines_cleared_buffer[4];            // Max 3 digits + null terminator

static volatile uint8_t render_tick_count = 0;
static volatile uint8_t rock_timer_hit = 0;

void play_space_invader() {
    init_game();
    game_active = 1;
    while (game_active) {
        update_user();
        if (rock_timer_hit) update_rock();
    }
}

void init_game() {
    oled_fill(0x00);
    oled_draw_user(user);
    rng_seed();
    generate_rock();
    lines_cleared = 0;
    slowness = 20;

    TCCR0A |= (1 << WGM01);                 // CTC mode
    TCCR0B |= (1 << CS02) | (1 << CS00);    // Prescaler 1024
    OCR0A = 249;                            // Compare value for 16ms
    TIMSK0 |= (1 << OCIE0A);                // Enable Timer0 Compare Match A interrupt
}

void generate_rock() {
    uint8_t random_x1 = rng_rand_range((SCREEN_WIDTH - ROCK_HOLE));
    Rock tmp_rock = { .hole_pos = random_x1, .old_pos = {0,0}, .pos = {0,0}};
    rock = tmp_rock;
    oled_draw_rock(rock);
}

void update_user() {
    bttns_states = bttns_read();
    if (bttns_states & (1 << BTTN1)) {
        user.old_pos = user.pos;
        if (user.pos.x <= 0) user.pos.x = 0;
        else user.pos.x -= USER_WIDTH;
        oled_draw_user(user);
    }
    else if (bttns_states & (1 << BTTN2)) {
        user.old_pos = user.pos;
        if (user.pos.x >= SCREEN_WIDTH - USER_WIDTH) user.pos.x = SCREEN_WIDTH - USER_WIDTH;
        else user.pos.x += USER_WIDTH;
        oled_draw_user(user);
    }
    else if (bttns_states & (1 << BTTN3)) {
        game_active = 0;
    }
}

void update_rock() {
    if (rock.pos.y == SCREEN_PAGES) {
        check_collision();
        oled_erase_rock(rock);
        generate_rock();
        check_level();
    }
    else {
        rock.old_pos = rock.pos;
        rock.pos.y++;
        oled_draw_rock(rock);
    }
    rock_timer_hit = 0;
}

void check_level() {
    lines_cleared++;
    if (lines_cleared % lines_per_level == 0) {
        slowness--;
    }
}

void check_collision() {
    if ((user.pos.x < rock.hole_pos) || ((user.pos.x + USER_WIDTH) > (rock.hole_pos + ROCK_HOLE))) {
        oled_fill(0x00);
        oled_set_cursor(0, 0);
        oled_write_string("Lines cleared: ");
        oled_set_cursor(0, 1);
        oled_write_string(itoa(lines_cleared, lines_cleared_buffer, 10));
        _delay_ms(2000);
        game_active = 0;
    }
}

ISR(TIMER0_COMPA_vect) {
    render_tick_count++;
    if (render_tick_count >= slowness) {    
        render_tick_count = 0;
        rock_timer_hit = 1;
    }
}

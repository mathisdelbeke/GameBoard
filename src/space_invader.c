#include "space_invader.h"
#include "display.h"
#include "buttons.h"
#include "rng.h"

static User user = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0, 7}, {0, 7}};
static Rock rocks[ACTIVE_ROCKS];
static uint8_t number_of_rocks = 0;

static uint8_t game_active = 0;
static uint8_t bttns_states;

void play_space_invader() {
    oled_fill(0x00);
    rng_seed();
    oled_draw_user(user);
    generate_rock();
    game_active = 1;
    while (game_active) {
        update_user();
        //update_rocks();
    }
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

void generate_rock() {
    uint8_t random_x = rng_rand_range();
    Rock rock = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {random_x, 0}, {random_x, 0}};
    rocks[number_of_rocks] = rock;
}
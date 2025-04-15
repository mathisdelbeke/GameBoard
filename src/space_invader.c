#include "space_invader.h"
#include "display.h"
#include "buttons.h"

static User user = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, {0, 7}, {0, 7}};

static uint8_t game_active = 0;
static uint8_t bttns_states;

void space_invader_start() {
    oled_fill(0x00);
    oled_draw_user(user);
    game_active = 1;
    while (game_active) {
        update_user();
        _delay_ms(100);
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
}
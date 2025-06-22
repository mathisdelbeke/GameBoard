#include "pathfinder.h"

#define SELECTOR_WIDTH 8
#define BLOCK_WIDTH 8
#define MAX_OBSTACLES ((SCREEN_PAGES * (SCREEN_WIDTH  / BLOCK_WIDTH)) - 2)

#define USER_SELECTS 3

typedef enum {
    SELECT_START,
    SELECT_DESTINATION,
    SELECT_OBSTACLES
}Game_state;

typedef struct {
    uint8_t x;
    uint8_t y;
}Pos;

static uint8_t game_active = 1;
static Game_state game_state = SELECT_START;

static uint8_t grid[SCREEN_PAGES][(SCREEN_WIDTH / BLOCK_WIDTH)];
static Pos obstacles_pos[MAX_OBSTACLES];   // Max all positions except start and destination
static uint8_t num_obstacles = 0;

static Pos selector_pos = {.x = 0, .y = 0};
static Pos start_pos;
static Pos destination_pos;

static void init_game();
static void proces_user_input();
static void user_selects();
static void try_select_start();
static void try_select_destination();
static void try_select_obstacle();
static void move_selector(uint8_t *bttn_states);
static void draw_selector(Pos *old_selector_pos);
static void try_erase_selector(Pos *old_selector_pos);
static void draw_waypoint(Pos* waypoint);

void play_pathfinder() {
    init_game();
    while (game_active) {
        proces_user_input();
    }
}

static void init_game() {
    oled_fill(0x00);
    start_pos = (Pos) {.x = 254, .y = 254};                                                         // Off-screen init value
    destination_pos = (Pos) {.x = 253, .y = 253};                                                   // Off-screen init value
    for (uint16_t i = 0; i < MAX_OBSTACLES; i++) obstacles_pos[i] = (Pos){.x = 252, .y = 252};      // All possible obstacles off-screen init value
    num_obstacles = 0;
    Pos old_selector_pos = {.x = 1, .y = 1};
    selector_pos = (Pos){.x = 0, .y = 0};
    draw_selector(&old_selector_pos);
    game_active = 1;
    game_state = SELECT_START;
}

static void proces_user_input() {
    uint8_t bttns_states = bttns_read();
    if (bttns_states == USER_SELECTS) user_selects();
    else if (bttns_states != 0) move_selector(&bttns_states);
}

static void user_selects() {
    if (game_state == SELECT_START) try_select_start(); 
    else if (game_state == SELECT_DESTINATION) try_select_destination();
    else if (game_state == SELECT_OBSTACLES) try_select_obstacle();
}

static void try_select_start() {
    start_pos = selector_pos;
    draw_waypoint(&start_pos);
    game_state = SELECT_DESTINATION;
}

static void try_select_destination() {
    if ((selector_pos.x != start_pos.x) || (selector_pos.y != start_pos.y)) {
        destination_pos = selector_pos;
        draw_waypoint(&destination_pos);
        game_state = SELECT_OBSTACLES;   
    }
}

static void try_select_obstacle() {
    if (num_obstacles < MAX_OBSTACLES) {
        if ((selector_pos.x == start_pos.x) && (selector_pos.y == start_pos.y)) return;
        if ((selector_pos.x == destination_pos.x) && (selector_pos.y == destination_pos.y)) return;
        for (uint8_t i = 0; i < num_obstacles; i++) {
            if ((selector_pos.x == obstacles_pos[i].x) && (selector_pos.y == obstacles_pos[i].y)) return;
        }
        num_obstacles++;
        obstacles_pos[num_obstacles - 1] = (Pos){.x = selector_pos.x, .y = selector_pos.y};
        draw_waypoint(&selector_pos);
    }
}

static void move_selector(uint8_t *bttns_states) {
    Pos old_selector_pos = selector_pos;
    if (*bttns_states & (1 << BTTN1)) {
        if (selector_pos.x >= SELECTOR_WIDTH) selector_pos.x -= SELECTOR_WIDTH; 
    }
    else if (*bttns_states & (1 << BTTN2)) {
        if (selector_pos.x < (SCREEN_WIDTH - SELECTOR_WIDTH)) selector_pos.x += SELECTOR_WIDTH;
    }
    else if (*bttns_states & (1 << BTTN3)) {
        if (selector_pos.y > 0)selector_pos.y -= 1;
    }
    else if (*bttns_states & (1 << BTTN4)) {
        if (selector_pos.y < (SCREEN_PAGES - 1)) selector_pos.y += 1;
    }
    draw_selector(&old_selector_pos);
}

static void draw_selector(Pos *old_selector_pos) {
    try_erase_selector(old_selector_pos);
    oled_set_cursor(selector_pos.x, selector_pos.y);
    for (uint8_t i = 0; i < SELECTOR_WIDTH; i++) {
        oled_send_data(0xFF);
    }
}

static void try_erase_selector(Pos *old_selector_pos) {
    if ((old_selector_pos->x == start_pos.x) && (old_selector_pos->y == start_pos.y)) return;
    if ((old_selector_pos->x == destination_pos.x) && (old_selector_pos->y == destination_pos.y)) return;
    for (uint8_t i = 0; i < num_obstacles; i++) {
        if ((old_selector_pos->x == obstacles_pos[i].x) && (old_selector_pos->y == obstacles_pos[i].y)) return;
    }

    oled_set_cursor(old_selector_pos->x, old_selector_pos->y);
    for (uint8_t i = 0; i < SELECTOR_WIDTH; i++) {
        oled_send_data(0x00);
    }   
}

static void draw_waypoint(Pos* waypoint) {
    oled_set_cursor(waypoint->x, waypoint->y);
    for (uint8_t i = 0; i < BLOCK_WIDTH; i++) {
        oled_send_data(0xFF);
    }
}

// Beter way to user_selects
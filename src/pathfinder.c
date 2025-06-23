#include "pathfinder.h"
#include <stdlib.h>

#define SELECTOR_WIDTH 8
#define BLOCK_WIDTH 8

#define MAX_BLOCKS_HOR (SCREEN_WIDTH / BLOCK_WIDTH)
#define MAX_BLOCKS_VER SCREEN_PAGES
#define MAX_BLOCKS (MAX_BLOCKS_VER * MAX_BLOCKS_HOR)
#define MAX_OBSTACLES 40

#define USER_SELECTS 3
#define USER_STARTS_ALGO 12

#define NUM_DIRECTIONS 4

typedef enum {
    SELECT_START,
    SELECT_DESTINATION,
    SELECT_OBSTACLES
}Game_state;

typedef struct {
    uint8_t x;
    uint8_t y;
}Pos;

typedef struct {
    uint8_t count;
    uint8_t head;
    uint8_t tail; 
    Pos elements[MAX_BLOCKS];
}Queue;

typedef struct {
    Pos path_found[MAX_BLOCKS];
    uint8_t path_count;
}Algo_result;


static uint8_t game_active;
static Game_state game_state;

static Pos obstacles_pos[MAX_OBSTACLES];               
static uint8_t num_obstacles;

static Pos selector_pos;
static Pos start_pos;
static Pos destination_pos;

static void init_game();
static void select();
static void try_select_start();
static void try_select_destination();
static void try_select_obstacle();
static void try_start_algo();
static void move_selector(uint8_t *bttn_states);
static void draw_selector(Pos *old_selector_pos);
static void try_erase_selector(Pos *old_selector_pos);
static void draw_waypoint(Pos* waypoint);
static void draw_obstacle(Pos *obstacle);
static void print_algo_result(Algo_result *algo_result);
static void bfs(Algo_result* algo_result);
static uint8_t is_obstacle(uint8_t x, uint8_t y);
static void write_queue(Queue *queue, Pos block_pos);
static void read_queue(Queue *queue, Pos *block_pos);

void play_pathfinder() {
    init_game();
    while (game_active) {
        uint8_t bttns_states = bttns_read();
        if (bttns_states == USER_SELECTS) select();
        if (bttns_states == USER_STARTS_ALGO) try_start_algo();
        else if (bttns_states != 0) move_selector(&bttns_states);
    }
}

static void init_game() {
    oled_fill(0x00);
    start_pos = (Pos) {.x = 16, .y = 16};                                                        // Off-screen init value
    destination_pos = (Pos) {.x = 16, .y = 16};                                                  // Off-screen init value
    for (uint8_t i = 0; i < MAX_OBSTACLES; i++) obstacles_pos[i] = (Pos){.x = 16, .y = 16};      // All possible obstacles off-screen init value
    num_obstacles = 0;
    Pos old_selector_pos = {.x = 16, .y = 16};                                                   // Off-screen init value 
    selector_pos = (Pos){.x = 0, .y = 0};
    draw_selector(&old_selector_pos);
    game_active = 1;
    game_state = SELECT_START;
}

static void select() {
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
        draw_obstacle(&selector_pos);
    }
}

static void try_start_algo() {
    if (game_state == SELECT_OBSTACLES) {
        try_erase_selector(&selector_pos);
        Algo_result algo_result = {.path_count = 0};
        bfs(&algo_result);
        print_algo_result(&algo_result);
        _delay_ms(3000);
        game_active = 0;
    }
}

static void move_selector(uint8_t *bttns_states) {
    Pos old_selector_pos = selector_pos;
    if (*bttns_states & (1 << BTTN1)) {
        if (selector_pos.x > 0) selector_pos.x -= 1; 
    }
    else if (*bttns_states & (1 << BTTN2)) {
        if (selector_pos.x < (MAX_BLOCKS_HOR - 1)) selector_pos.x += 1;
    }
    else if (*bttns_states & (1 << BTTN3)) {
        if (selector_pos.y > 0) selector_pos.y -= 1;
    }
    else if (*bttns_states & (1 << BTTN4)) {
        if (selector_pos.y < (MAX_BLOCKS_VER - 1)) selector_pos.y += 1;
    }
    draw_selector(&old_selector_pos);  
}

static void draw_selector(Pos *old_selector_pos) {
    try_erase_selector(old_selector_pos);
    oled_set_cursor((selector_pos.x * SELECTOR_WIDTH), selector_pos.y);
    for (uint8_t i = 0; i < SELECTOR_WIDTH; i++) {
        oled_send_data(0xFF);
    }
}

static void try_erase_selector(Pos *old_selector_pos) {
    if ((old_selector_pos->x == start_pos.x) && (old_selector_pos->y == start_pos.y)) return;
    if ((old_selector_pos->x == destination_pos.x) && (old_selector_pos->y == destination_pos.y)) return;
    
    for (uint8_t i = 0; i < num_obstacles; i++) {
        if ((old_selector_pos->x == obstacles_pos[i].x) && (old_selector_pos->y == obstacles_pos[i].y)) {
            oled_set_cursor((old_selector_pos->x * SELECTOR_WIDTH), old_selector_pos->y);
            for (uint8_t j = 0; j < SELECTOR_WIDTH; j++) {
                oled_send_data(0x00);
            }
            draw_obstacle(old_selector_pos);
            return;             
        }
    }
    oled_set_cursor((old_selector_pos->x * SELECTOR_WIDTH), old_selector_pos->y);
    for (uint8_t i = 0; i < SELECTOR_WIDTH; i++) {
        oled_send_data(0x00);
    }   
}

static void draw_waypoint(Pos *waypoint) {
    oled_set_cursor((waypoint->x * BLOCK_WIDTH), waypoint->y);
    for (uint8_t i = 0; i < BLOCK_WIDTH; i++) {
        oled_send_data(0xFF);
    }
}

static void draw_obstacle(Pos *obstacle) {
    oled_set_cursor(((obstacle->x * BLOCK_WIDTH) + 2), obstacle->y);
    for (uint8_t i = 0; i < (BLOCK_WIDTH - 4); i++) {
        oled_send_data(0x3C);
    }    
}

static void print_algo_result(Algo_result *algo_result) {
    if (algo_result->path_count != 0) {
        for (uint8_t i = 0; i < algo_result->path_count; i++) {
            oled_set_cursor((algo_result->path_found[i].x * BLOCK_WIDTH), algo_result->path_found[i].y);
            for (uint8_t j = 0; j < BLOCK_WIDTH; j++) {
                oled_send_data(0xFF);
            }
            _delay_ms(100);
        }
    }
    else {
        oled_fill(0x00);
        oled_set_cursor(0, 0);
        oled_write_string("No path found");
    }
}

static void bfs(Algo_result *algo_result) {
    static Pos parent[MAX_BLOCKS_VER][MAX_BLOCKS_HOR];                          // Static to save stack memory usage
    static uint8_t visited[MAX_BLOCKS_VER][MAX_BLOCKS_HOR];                     // Static to save stack memory usage
    for (uint8_t i = 0; i < MAX_BLOCKS_VER; i++) {                              // Reset static vars
        for (uint8_t j = 0; j < MAX_BLOCKS_HOR; j++) {
            parent[i][j] = (Pos){.x = 0, .y = 0};
            visited[i][j] = 0;
        }
    }

    uint8_t path_idx = 0;
    uint8_t directions[NUM_DIRECTIONS][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    Queue queue = {.count = 0, .head = 0, .tail = 0};
    write_queue(&queue, start_pos);
    visited[start_pos.y][start_pos.x] = 1;

    while (queue.count > 0) {
        Pos block_pos;
        read_queue(&queue, &block_pos);
        for (uint8_t i = 0; i < NUM_DIRECTIONS; i++) {                           
            uint8_t nx = block_pos.x + directions[i][0];
            uint8_t ny = block_pos.y + directions[i][1];
            if ((nx >= 0) && (nx < MAX_BLOCKS_HOR) && ((ny >= 0) && (ny < MAX_BLOCKS_VER))) {
                if ((!visited[ny][nx]) && (!is_obstacle(nx, ny))) {
                    visited[ny][nx] = 1;
                    parent[ny][nx] = block_pos;
                    Pos new_block_pos = (Pos){.x = nx, .y = ny};
                    write_queue(&queue, new_block_pos);
                    if ((nx == destination_pos.x) && (ny == destination_pos.y)) {
                        while ((nx != start_pos.x) || (ny != start_pos.y)) {
                            algo_result->path_found[path_idx] = new_block_pos;
                            new_block_pos = parent[ny][nx]; 
                            ny = new_block_pos.y;
                            nx = new_block_pos.x;
                            path_idx++;
                        }
                        algo_result->path_found[path_idx] = start_pos;
                        algo_result->path_count = path_idx + 1;
                        return;
                    }
                }
            }
        }
    }
}

static uint8_t is_obstacle(uint8_t x, uint8_t y) {
    for (uint8_t i = 0; i < num_obstacles; i++) {
        if ((x == obstacles_pos[i].x) && (y == obstacles_pos[i].y)) return 1;
    }
    return 0;
}

static void write_queue(Queue *queue, Pos block_pos) {
    if (queue->count < MAX_BLOCKS) {
        queue->elements[queue->head] = block_pos;
        queue->head = (queue->head + 1) % MAX_BLOCKS;
        queue->count++;
    }
}

static void read_queue(Queue *queue, Pos *block_pos) {
    if (queue->count > 0) {
        *block_pos = queue->elements[queue->tail];
        queue->tail = (queue->tail + 1) % MAX_BLOCKS;
        queue->count--;
    }
}
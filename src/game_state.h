#pragma once
#include "constants.h"

typedef struct {
    int16_t life_totals[MAX_PLAYERS];
    uint32_t round_time_left;
    uint32_t last_run;
} GameState;

GameState *game_state_load(void);
void game_state_destroy(GameState *this);

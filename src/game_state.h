#pragma once
#include "constants.h"

typedef struct {
    int16_t life_totals[MAX_PLAYERS];
} GameState;

GameState *game_state_load(void);
void game_state_save(GameState *this);
void game_state_destroy(GameState *this);

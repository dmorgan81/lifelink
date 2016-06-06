#pragma once
#include <pebble.h>
#include "game_state.h"

typedef Layer RoundTimerLayer;

RoundTimerLayer *round_timer_layer_create(GRect frame, GameState *game_state);
void round_timer_layer_destory(RoundTimerLayer *this);
void round_timer_layer_tick(RoundTimerLayer *this);

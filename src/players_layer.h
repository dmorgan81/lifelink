#pragma once
#include <pebble.h>
#include "game_state.h"

typedef Layer PlayersLayer;

PlayersLayer *players_layer_create(GRect frame, GameState *game_state);
void players_layer_destroy(PlayersLayer *this);
void players_layer_swap_players(PlayersLayer *this);
void players_layer_current_player_increment_life(PlayersLayer *this);
void players_layer_current_player_decrement_life(PlayersLayer *this);

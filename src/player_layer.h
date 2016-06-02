#pragma once
#include <pebble.h>

typedef Layer PlayerLayer;

PlayerLayer *player_layer_create(GRect frame);
void player_layer_destroy(PlayerLayer *this);
int16_t player_layer_get_life(PlayerLayer *this);
void player_layer_set_life(PlayerLayer *this, int16_t life);
void player_layer_set_name(PlayerLayer *this, char *name);

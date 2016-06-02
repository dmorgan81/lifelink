#pragma once
#include <pebble.h>

typedef Layer PlayersLayer;

PlayersLayer *players_layer_create(GRect frame);
void players_layer_destroy(PlayersLayer *this);

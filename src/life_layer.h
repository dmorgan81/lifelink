#pragma once
#include <pebble.h>

typedef Layer LifeLayer;

LifeLayer *life_layer_create(GRect frame);
void life_layer_destroy(LifeLayer *this);
int16_t life_layer_get_life(LifeLayer *this);
void life_layer_set_life(LifeLayer *this, int16_t life);

#pragma once
#include <pebble.h>

#define ROUND_TIME_LAYER_HEIGHT 16
#define ROUND_TIME_DEFAULT_LENGTH 3000000

typedef struct RoundTimeLayer {
    Layer *layer;
    TextLayer *text_layer;
    long time_left;
} RoundTimeLayer;

RoundTimeLayer *round_time_layer_create();
void round_time_layer_destroy(RoundTimeLayer *round_time_layer);
Layer *round_time_layer_get_layer(RoundTimeLayer *round_time_layer);
void round_time_layer_set_frame(RoundTimeLayer *round_time_layer, GRect frame);
void round_time_layer_set_time_left(RoundTimeLayer *round_time_layer, long time_left);
long round_time_layer_tick(RoundTimeLayer *round_time_layer);

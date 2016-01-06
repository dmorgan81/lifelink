#pragma once
#include <pebble.h>
#include "round_time_layer.h"
#include "layout.h"

#define ROUND_TIMER_DEFAULT_LENGTH 3000000

typedef struct RoundTimer {
    RoundTimeLayer *layer;
    uint32_t time_left;
    bool enabled;
} RoundTimer;

RoundTimer *round_timer_create(LayoutGroup *layout_group);
void round_timer_destroy(RoundTimer *round_timer);
uint32_t round_timer_tick(RoundTimer *round_timer);
void round_timer_reset(RoundTimer *round_timer);
void round_timer_set_enabled(RoundTimer *round_timer, bool enabled);

#include <pebble.h>
#include "round_timer.h"
#include "round_time_layer.h"
#include "layout.h"

RoundTimer *round_timer_create(LayoutGroup *layout_group) {
    RoundTimer *round_timer = malloc(sizeof(RoundTimer));
    round_timer->layer = layout_group->round_time_layer;

    bool enabled = persist_read_bool(STORAGE_ROUND_TIMER_ENABLED);
     if (enabled && persist_exists(STORAGE_ROUND_TIME_KEY) && persist_exists(STORAGE_LAST_RUN_KEY)) {
         int32_t time_left = persist_read_int(STORAGE_ROUND_TIME_KEY);
         int32_t last_run = persist_read_int(STORAGE_LAST_RUN_KEY) * 1000;
         int32_t now = time(NULL) * 1000;
         time_left -= (now - last_run);
         if (time_left > 0) round_timer->time_left = time_left;
    } else {
        round_timer->time_left = ROUND_TIMER_DEFAULT_LENGTH;
    }
    round_timer_set_enabled(round_timer, enabled);

    round_time_layer_set_time_left(round_timer->layer, round_timer->time_left);
    return round_timer;
}

void round_timer_destroy(RoundTimer *round_timer) {
    persist_write_int(STORAGE_ROUND_TIME_KEY, round_timer->time_left);
    persist_write_int(STORAGE_LAST_RUN_KEY, time(NULL));
    persist_write_bool(STORAGE_ROUND_TIMER_ENABLED, round_timer->enabled);
    free(round_timer);
}

uint32_t round_timer_tick(RoundTimer *round_timer) {
    round_timer->time_left -= 1000;
    round_time_layer_set_time_left(round_timer->layer, round_timer->time_left);
    return round_timer->time_left;
}

void round_timer_reset(RoundTimer *round_timer) {
    round_timer->time_left = ROUND_TIMER_DEFAULT_LENGTH;
    round_time_layer_set_time_left(round_timer->layer, round_timer->time_left);
}

void round_timer_set_enabled(RoundTimer *round_timer, bool enabled) {
    layer_set_hidden(round_time_layer_get_layer(round_timer->layer), !enabled);
    round_timer->enabled = enabled;
    if (!enabled) round_timer_reset(round_timer);
}

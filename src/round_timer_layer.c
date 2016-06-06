#include <pebble.h>
#include "logging.h"
#include "constants.h"
#include "settings.h"
#include "game_state.h"
#include "round_timer_layer.h"

typedef struct {
    char buf[8];
    TextLayer *text_layer;
    GameState *game_state;
} Data;

static void settings_update_listener(Settings *settings, void *context) {
    log_func();
    RoundTimerLayer *this = (RoundTimerLayer *) context;
    Data *data = (Data *) layer_get_data(this);

    if (!settings->round_timer_enabled) {
        data->game_state->round_time_left = DEFAULT_ROUND_TIME;
        data->game_state->last_run = 0;
    } else if (data->game_state->last_run > 0) {
        int32_t time_left = data->game_state->round_time_left;
        int32_t last_run = data->game_state->last_run * 1000;
        int32_t now = time(NULL) * 1000;
        time_left -= (now - last_run);
        data->game_state->round_time_left = time_left > 0 ? time_left : DEFAULT_ROUND_TIME;
    }
    layer_set_hidden(this, !settings->round_timer_enabled);
}

RoundTimerLayer *round_timer_layer_create(GRect frame, GameState *game_state) {
    log_func();
    RoundTimerLayer *this = layer_create_with_data(frame, sizeof(Data));
    Data *data = (Data *) layer_get_data(this);
    GRect bounds = layer_get_bounds(this);

    data->game_state = game_state;

    data->text_layer = text_layer_create(bounds);
    text_layer_set_font(data->text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(data->text_layer, GTextAlignmentCenter);
    text_layer_set_text_color(data->text_layer, GColorWhite);
    text_layer_set_background_color(data->text_layer, GColorBlack);
    text_layer_set_text(data->text_layer, data->buf);
    layer_add_child(this, text_layer_get_layer(data->text_layer));

    settings_add_listener(settings_update_listener, this);
    return this;
}

void round_timer_layer_destory(RoundTimerLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    text_layer_destroy(data->text_layer);
    layer_destroy(this);
}

void round_timer_layer_tick(RoundTimerLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    data->game_state->round_time_left -= 1000;
    int min = (data->game_state->round_time_left % (1000 * 60 * 60)) / (1000 * 60);
    int sec = ((data->game_state->round_time_left % (1000 * 60 * 60)) % (1000 * 60)) / 1000;
    snprintf(data->buf, sizeof(data->buf), "%s%d:%s%d", (min < 10 ? "0" : ""), min, (sec < 10 ? "0" : ""), sec);
    layer_mark_dirty(this);
}

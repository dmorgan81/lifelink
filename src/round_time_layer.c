#include <pebble.h>
#include "round_time_layer.h"

#define _ROUND_TIME_BUF_LENGTH 6

RoundTimeLayer *round_time_layer_create() {
    RoundTimeLayer *round_time_layer = malloc(sizeof(RoundTimeLayer));

    Layer *layer = layer_create(GRectZero);
    round_time_layer->layer = layer;

    TextLayer *text_layer = text_layer_create(GRectZero);
    text_layer_set_text_alignment(text_layer, GTextAlignmentRight);
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_background_color(text_layer, GColorClear);
    text_layer_set_text_color(text_layer, GColorWhite);
    round_time_layer->text_layer = text_layer;

    layer_add_child(layer, text_layer_get_layer(text_layer));
    round_time_layer_set_time_left(round_time_layer, ROUND_TIME_DEFAULT_LENGTH);

    return round_time_layer;
}

void round_time_layer_destroy(RoundTimeLayer *round_time_layer) {
    text_layer_destroy(round_time_layer->text_layer);
    layer_destroy(round_time_layer->layer);
    free(round_time_layer);
}

Layer *round_time_layer_get_layer(RoundTimeLayer *round_time_layer) {
    return round_time_layer->layer;
}

void round_time_layer_set_frame(RoundTimeLayer *round_time_layer, GRect frame) {
    Layer *layer = round_time_layer_get_layer(round_time_layer);
    layer_set_frame(layer, frame);
    layer_set_frame(text_layer_get_layer(round_time_layer->text_layer), GRect(0, 0, frame.size.w, frame.size.h - 2));
    layer_set_frame(round_time_layer_get_layer(round_time_layer), frame);
}

static void round_time_layer_update(RoundTimeLayer *round_time_layer) {
    long time_left = round_time_layer->time_left;
    static char buf[_ROUND_TIME_BUF_LENGTH];
    int min = (time_left % (1000 * 60 * 60)) / (1000 * 60);
    int sec = ((time_left % (1000 * 60 * 60)) % (1000 * 60)) / 1000;
    snprintf(buf, sizeof(buf), "%s%d:%s%d",
        (min < 10 ? "0" : ""), min, (sec < 10 ? "0" : ""), sec);
    text_layer_set_text(round_time_layer->text_layer, buf);
}

void round_time_layer_set_time_left(RoundTimeLayer *round_time_layer, long time_left) {
    round_time_layer->time_left = time_left;
    round_time_layer_update(round_time_layer);
}

long round_time_layer_tick(RoundTimeLayer *round_time_layer) {
    round_time_layer->time_left -= 1000;
    round_time_layer_update(round_time_layer);
    return round_time_layer->time_left;
}

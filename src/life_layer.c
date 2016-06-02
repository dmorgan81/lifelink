#include <pebble.h>
#include "logging.h"
#include "life_layer.h"

#define LIFE_LAYER_MAX_BUF_LEN 8

typedef struct {
    char buf[LIFE_LAYER_MAX_BUF_LEN];
    int16_t life;
    TextLayer *text_layer;
} Data;

LifeLayer *life_layer_create(GRect frame) {
    log_func();
    LifeLayer *this = layer_create_with_data(frame, sizeof(Data));
    Data *data = (Data *) layer_get_data(this);
    GRect bounds = layer_get_bounds(this);

    data->text_layer = text_layer_create(bounds);
    text_layer_set_font(data->text_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
    text_layer_set_text_alignment(data->text_layer, GTextAlignmentRight);
    text_layer_set_text(data->text_layer, data->buf);
    layer_add_child(this, text_layer_get_layer(data->text_layer));

    return this;
}

void life_layer_destroy(LifeLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    text_layer_destroy(data->text_layer);
    layer_destroy(this);
}

int16_t layer_get_life(LifeLayer *this) {
    log_func();
    return ((Data *) layer_get_data(this))->life;
}

void life_layer_set_life(LifeLayer *this, int16_t life) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    data->life = life;
    snprintf(data->buf, LIFE_LAYER_MAX_BUF_LEN, "%d", life);
    layer_mark_dirty(this);
}

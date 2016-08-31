#include <pebble.h>
#include "logging.h"
#include "constants.h"
#include "name_layer.h"

typedef struct {
    char buf[MAX_NAME_LEN];
    TextLayer *text_layer;
} Data;

NameLayer *name_layer_create(GRect frame) {
    log_func();
    NameLayer *this = layer_create_with_data(frame, sizeof(Data));
    Data *data = (Data *) layer_get_data(this);
    GRect bounds = layer_get_bounds(this);

    data->text_layer = text_layer_create(bounds);
    text_layer_set_text_color(data->text_layer, GColorWhite);
    text_layer_set_background_color(data->text_layer, GColorBlack);
    text_layer_set_text_alignment(data->text_layer, GTextAlignmentRight);
    text_layer_set_text(data->text_layer, data->buf);
    layer_add_child(this, text_layer_get_layer(data->text_layer));

    return this;
}

void name_layer_destroy(NameLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    text_layer_destroy(data->text_layer);
    layer_destroy(this);
}

char *name_layer_get_name(NameLayer *this) {
    log_func();
    return ((Data *) layer_get_data(this))->buf;
}

void name_layer_set_name(NameLayer *this, const char *name) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    strncpy(data->buf, name, MAX_NAME_LEN - 1);
    layer_mark_dirty(this);
}
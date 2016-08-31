#include <pebble.h>
#include "logging.h"
#include "life_layer.h"
#include "name_layer.h"
#include "player_layer.h"

typedef struct {
    LifeLayer *life_layer;
    NameLayer *name_layer;
} Data;

PlayerLayer *player_layer_create(GRect frame) {
    log_func();
    PlayerLayer *this = layer_create_with_data(frame, sizeof(Data));
    Data *data = (Data *) layer_get_data(this);
    GRect bounds = layer_get_bounds(this);

    GRect inset = grect_inset(bounds, GEdgeInsets(0, 5, 29));
    data->life_layer = life_layer_create(inset);
    layer_add_child(this, data->life_layer);

    inset= grect_inset(bounds, GEdgeInsets(47, 5, 5));
    data->name_layer = name_layer_create(inset);
    layer_add_child(this, data->name_layer);

    return this;
}

void player_layer_destroy(PlayerLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    name_layer_destroy(data->name_layer);
    life_layer_destroy(data->life_layer);
    layer_destroy(this);
}

int16_t player_layer_get_life(PlayerLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    return life_layer_get_life(data->life_layer);
}

void player_layer_set_life(PlayerLayer *this, int16_t life) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    life_layer_set_life(data->life_layer, life);
}

char *player_layer_get_name(PlayerLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    return name_layer_get_name(data->name_layer);
}

void player_layer_set_name(PlayerLayer *this, const char *name) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    name_layer_set_name(data->name_layer, name);
    layer_mark_dirty(this);
}

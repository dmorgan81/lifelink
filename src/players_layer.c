#include <pebble.h>
#include "logging.h"
#include "player_layer.h"
#include "players_layer.h"

#define PLAYERS_LAYER_MAX_PLAYERS 2

typedef struct {
    PlayerLayer *player_layers[PLAYERS_LAYER_MAX_PLAYERS];
} Data;

PlayersLayer *players_layer_create(GRect frame) {
    log_func();
    PlayersLayer *this = layer_create_with_data(frame, sizeof(Data));
    Data *data = (Data *) layer_get_data(this);
    GRect bounds = layer_get_bounds(this);

    uint8_t h = bounds.size.h / 2;
    GRect inset = GRect(0, 0, bounds.size.w, h);
    data->player_layers[0] = player_layer_create(inset);
    player_layer_set_name(data->player_layers[0], "Player One");
    player_layer_set_life(data->player_layers[0], 20);
    layer_add_child(this, data->player_layers[0]);

    inset = GRect(0, h, bounds.size.w, h);
    data->player_layers[1] = player_layer_create(inset);
    player_layer_set_name(data->player_layers[1], "Player Two");
    player_layer_set_life(data->player_layers[1], 20);
    layer_add_child(this, data->player_layers[1]);

    return this;
}

void players_layer_destroy(PlayersLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    for (uint8_t i = 0; i < PLAYERS_LAYER_MAX_PLAYERS; i++) {
        player_layer_destroy(data->player_layers[i]);
    }
    layer_destroy(this);
}

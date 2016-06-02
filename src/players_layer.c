#include <pebble.h>
#include "logging.h"
#include "constants.h"
#include "game_state.h"
#include "effect_layer/effect_layer.h"
#include "player_layer.h"
#include "players_layer.h"

typedef struct {
    PlayerLayer *player_layers[MAX_PLAYERS];
    PlayerLayer *current_player;
    InverterLayer *inverter_layer;
    GameState *game_state;
} Data;

PlayersLayer *players_layer_create(GRect frame, GameState *game_state) {
    log_func();
    PlayersLayer *this = layer_create_with_data(frame, sizeof(Data));
    Data *data = (Data *) layer_get_data(this);
    GRect bounds = layer_get_bounds(this);

    uint8_t h = bounds.size.h / 2;
    GRect inset = GRect(0, 0, bounds.size.w, h);
    data->player_layers[0] = player_layer_create(inset);
    player_layer_set_life(data->player_layers[0], game_state->life_totals[0]);
    player_layer_set_name(data->player_layers[0], "Player One");
    layer_add_child(this, data->player_layers[0]);

    inset = GRect(0, h, bounds.size.w, h);
    data->player_layers[1] = player_layer_create(inset);
    player_layer_set_life(data->player_layers[1], game_state->life_totals[1]);
    player_layer_set_name(data->player_layers[1], "Player Two");
    layer_add_child(this, data->player_layers[1]);

    data->current_player = data->player_layers[0];

    inset = layer_get_bounds(data->current_player);
    data->inverter_layer = inverter_layer_create(inset);
    layer_add_child(this, inverter_layer_get_layer(data->inverter_layer));

    data->game_state = game_state;

    return this;
}

void players_layer_destroy(PlayersLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    for (uint8_t i = 0; i < MAX_PLAYERS; i++) {
        data->game_state->life_totals[i] = player_layer_get_life(data->player_layers[i]);
        player_layer_destroy(data->player_layers[i]);
    }
    inverter_layer_destroy(data->inverter_layer);
    layer_destroy(this);
}

void players_layer_swap_players(PlayersLayer *this) {
    log_func();
    Data *data = (Data *) layer_get_data(this);

    uint8_t i;
    for (i = 0; i < MAX_PLAYERS; i++) {
        if (data->current_player == data->player_layers[i]) {
            break;
        }
    }
    i = (i == MAX_PLAYERS - 1) ? 0 : i + 1;

    logd("before %s", player_layer_get_name(data->current_player));
    data->current_player = data->player_layers[i];
    logd("after %s", player_layer_get_name(data->current_player));

    GRect frame = layer_get_frame(data->current_player);
    PropertyAnimation *property_animation = property_animation_create_layer_frame(inverter_layer_get_layer(data->inverter_layer), NULL, &frame);
    Animation *animation = property_animation_get_animation(property_animation);
    animation_schedule(animation);
}

static void current_player_modify_life(PlayersLayer *this, int16_t i) {
    log_func();
    Data *data = (Data *) layer_get_data(this);
    int16_t life = player_layer_get_life(data->current_player);
    player_layer_set_life(data->current_player, life + i);
    layer_mark_dirty(this);
}

void players_layer_current_player_increment_life(PlayersLayer *this) {
    log_func();
    current_player_modify_life(this, 1);
}

void players_layer_current_player_decrement_life(PlayersLayer *this) {
    log_func();
    current_player_modify_life(this, -1);
}

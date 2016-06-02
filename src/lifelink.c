#include <pebble.h>
#include "logging.h"
#include "players_layer.h"

static Window *s_window;
static ActionBarLayer *s_action_bar_layer;
static StatusBarLayer *s_status_bar_layer;
static PlayersLayer *s_players_layer;

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    log_func();
    players_layer_current_player_increment_life(s_players_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    log_func();
    players_layer_swap_players(s_players_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    log_func();
    players_layer_current_player_decrement_life(s_players_layer);
}

static void click_config_provider(void *context) {
    log_func();
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 50, up_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 50, down_click_handler);
}

static void window_load(Window *window) {
    log_func();
    window_set_background_color(window, GColorBlack);
    Layer *root_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root_layer);

    s_action_bar_layer = action_bar_layer_create();
    action_bar_layer_set_click_config_provider(s_action_bar_layer, click_config_provider);
    action_bar_layer_add_to_window(s_action_bar_layer, window);

    uint8_t w = bounds.size.w - ACTION_BAR_WIDTH;
    GRect frame = GRect(0, 0, w, STATUS_BAR_LAYER_HEIGHT);
    s_status_bar_layer = status_bar_layer_create();
    layer_set_frame(status_bar_layer_get_layer(s_status_bar_layer), frame);
    layer_add_child(root_layer, status_bar_layer_get_layer(s_status_bar_layer));

    frame = GRect(0, STATUS_BAR_LAYER_HEIGHT, w, bounds.size.h - STATUS_BAR_LAYER_HEIGHT);
    s_players_layer = players_layer_create(frame);
    layer_add_child(root_layer, s_players_layer);
}

static void window_unload(Window *window) {
    log_func();
    players_layer_destroy(s_players_layer);
    status_bar_layer_destroy(s_status_bar_layer);
    action_bar_layer_destroy(s_action_bar_layer);
}

static void init(void) {
    log_func();
    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    window_stack_push(s_window, true);
}

static void deinit(void) {
    log_func();
    window_destroy(s_window);
}

int main(void) {
    log_func();
    init();
    app_event_loop();
    deinit();
}

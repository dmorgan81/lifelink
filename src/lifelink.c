#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include "logging.h"
#include "game_state.h"
#include "sync.h"
#include "enamel.h"
#include "players_layer.h"
#include "round_timer_layer.h"

static GameState *s_game_state;

static EventHandle s_tick_timer_event_handle;
static EventHandle s_settings_event_handle;

static Window *s_window;
static GBitmap *s_action_bar_icons[3];
static ActionBarLayer *s_action_bar_layer;
static StatusBarLayer *s_status_bar_layer;
static PlayersLayer *s_players_layer;
static RoundTimerLayer *s_round_timer_layer;

typedef enum {
    ActionBarIconUp,
    ActionBarIconSwap,
    ActionBarIconDown,
    ActionBarIconCount
} ActionBarIcons;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    log_func();
    round_timer_layer_tick(s_round_timer_layer);
    if (s_game_state->round_time_left <= 0) {
        tick_timer_service_unsubscribe();
        vibes_double_pulse();
    }
}

static void settings_update_handler(void *context) {
    log_func();
    if (enamel_get_RoundTimerEnabled()) {
        s_tick_timer_event_handle = events_tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    } else if (s_tick_timer_event_handle != NULL) {
        events_tick_timer_service_unsubscribe(s_tick_timer_event_handle);
    }
}

static void select_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
    log_func();
    players_layer_reset(s_players_layer);
    vibes_short_pulse();
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
    log_func();
    if (enamel_get_RoundTimerEnabled()) {
        players_layer_reset(s_players_layer);
        s_game_state->round_time_left = enamel_get_RoundLength();
        vibes_long_pulse();
    }
}

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
    window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 0, 0, true, select_multi_click_handler);
    window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 50, up_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 50, down_click_handler);
}

static void window_load(Window *window) {
    log_func();
    window_set_background_color(window, GColorBlack);
    Layer *root_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root_layer);

    s_action_bar_layer = action_bar_layer_create();
    s_action_bar_icons[ActionBarIconUp] = gbitmap_create_with_resource(RESOURCE_ID_UP);
    s_action_bar_icons[ActionBarIconSwap] = gbitmap_create_with_resource(RESOURCE_ID_SWAP);
    s_action_bar_icons[ActionBarIconDown] = gbitmap_create_with_resource(RESOURCE_ID_DOWN);
    action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_action_bar_icons[ActionBarIconUp]);
    action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_SELECT, s_action_bar_icons[ActionBarIconSwap]);
    action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_DOWN, s_action_bar_icons[ActionBarIconDown]);
    action_bar_layer_set_icon_press_animation(s_action_bar_layer, BUTTON_ID_UP, ActionBarLayerIconPressAnimationMoveUp);
    action_bar_layer_set_icon_press_animation(s_action_bar_layer, BUTTON_ID_DOWN, ActionBarLayerIconPressAnimationMoveDown);
    action_bar_layer_set_click_config_provider(s_action_bar_layer, click_config_provider);
    action_bar_layer_add_to_window(s_action_bar_layer, window);

    s_status_bar_layer = status_bar_layer_create();
    layer_add_child(root_layer, status_bar_layer_get_layer(s_status_bar_layer));

    GRect frame = GRect(0, -2, bounds.size.w, STATUS_BAR_LAYER_HEIGHT + 2);
    s_round_timer_layer = round_timer_layer_create(frame, s_game_state);
    layer_add_child(status_bar_layer_get_layer(s_status_bar_layer), s_round_timer_layer);

    frame = GRect(0, STATUS_BAR_LAYER_HEIGHT, bounds.size.w - ACTION_BAR_WIDTH, bounds.size.h - STATUS_BAR_LAYER_HEIGHT);
    s_players_layer = players_layer_create(frame, s_game_state);
    layer_add_child(root_layer, s_players_layer);

    s_settings_event_handle = enamel_settings_received_subscribe(settings_update_handler, NULL);
}

static void window_unload(Window *window) {
    log_func();
    enamel_settings_received_unsubscribe(s_settings_event_handle);
    if (s_tick_timer_event_handle) {
        events_tick_timer_service_unsubscribe(s_tick_timer_event_handle);
    }

    players_layer_destroy(s_players_layer);
    status_bar_layer_destroy(s_status_bar_layer);
    action_bar_layer_destroy(s_action_bar_layer);
    round_timer_layer_destory(s_round_timer_layer);
    for (uint8_t i = 0; i < ActionBarIconCount; i++) {
        gbitmap_destroy(s_action_bar_icons[i]);
    }
}

static void focus_handler(bool focus) {
    log_func();
    if (focus) {
        settings_update_handler(NULL);
        app_focus_service_unsubscribe();
    }
}

static void init(void) {
    log_func();
    sync_init();
    s_game_state = game_state_load();

    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

    app_focus_service_subscribe_handlers((AppFocusHandlers) {
        .did_focus = focus_handler
    });

    window_stack_push(s_window, true);
}

static void deinit(void) {
    log_func();
    window_destroy(s_window);

    game_state_destroy(s_game_state);
    sync_deinit();
}

int main(void) {
    log_func();
    init();
    app_event_loop();
    deinit();
}

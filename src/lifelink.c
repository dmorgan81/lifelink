#include <pebble.h>

#include "layout.h"
#include "player.h"
#include "action_bar.h"
#include "config_listener.h"

static Window *main_window;
static ActionBar *action_bar;
static LayoutGroup *layout_group;
static ConfigListener *config_listener;

static Player *player_one;
static Player *player_two;
static Player *current_player;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    current_player = (current_player == player_one ? player_two : player_one);
    layout_group_select_player(layout_group, current_player);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    player_modify_life(current_player, 1);
    layout_group_update_player(layout_group, current_player);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    player_modify_life(current_player, -1);
    layout_group_update_player(layout_group, current_player);
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
    player_set_life(player_one, PLAYER_STARTING_LIFE);
    player_set_life(player_two, PLAYER_STARTING_LIFE);
    current_player = player_one;
    layout_group_select_player(layout_group, current_player);
    layout_group_mark_dirty(layout_group);
    vibes_short_pulse();
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);

    window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    long time_left = layout_group_round_time_tick(layout_group);
    if (time_left <= 0) {
        tick_timer_service_unsubscribe();
        vibes_long_pulse();
    }
}

static void main_window_load(Window *window) {
    window_set_background_color(window, GColorBlack);

    if (player_exists_in_storage(PLAYER_ONE_STORAGE_KEY))
        player_one = player_read_from_storage(PLAYER_ONE_STORAGE_KEY);
    else
        player_one = player_create("Player One");

    if (player_exists_in_storage(PLAYER_TWO_STORAGE_KEY))
        player_two = player_read_from_storage(PLAYER_TWO_STORAGE_KEY);
    else
        player_two = player_create("Player Two");

    current_player = player_one;

    action_bar = action_bar_create();
    action_bar_add_to_window(action_bar, window);
    action_bar_set_click_config_provider(action_bar, click_config_provider);

    layout_group = layout_group_create(player_one, player_two);
    layout_group_add_to_window(layout_group, window);

    config_listener = config_listener_create(layout_group);
}

static void main_window_unload(Window *window) {
    tick_timer_service_unsubscribe();

    config_listener_destroy(config_listener);

    action_bar_destroy(action_bar);

    layout_group_destroy(layout_group);

    player_write_to_storage(player_one, PLAYER_ONE_STORAGE_KEY);
    player_write_to_storage(player_two, PLAYER_TWO_STORAGE_KEY);

    player_destroy(player_one);
    player_destroy(player_two);
}

static void focus_handler(bool focus) {
    if (focus) {
        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
        app_focus_service_unsubscribe();
    }
}

static void handle_init() {
    main_window = window_create();
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    app_focus_service_subscribe_handlers((AppFocusHandlers) {
        .did_focus = focus_handler
    });

    window_stack_push(main_window, true);
}

static void handle_deinit() {
    window_destroy(main_window);
}

int main() {
    handle_init();
    app_event_loop();
    handle_deinit();
}

#include <pebble.h>

#include "layout.h"
#include "player.h"
#include "action_bar.h"
#include "round_timer.h"

#define _CONFIG_LISTENER_SYNC_BUFFER_LEN 124

typedef enum {
    CONFIG_KEY_PLAYER_ONE_NAME = 0x1,
    CONFIG_KEY_PLAYER_TWO_NAME = 0x2,
    CONFIG_KEY_ROUND_TIME_ENABLED = 0x3
} ConfigListenerKeys;

typedef struct ConfigListener {
    AppSync *app_sync;
    uint8_t *sync_buffer;
} ConfigListener;

static Window *main_window;
static ActionBar *action_bar;
static LayoutGroup *layout_group;
static RoundTimer *round_timer;
static ConfigListener *config_listener;

static Player *player_one;
static Player *player_two;
static Player *current_player;

static void reset_game() {
    player_set_life(player_one, PLAYER_STARTING_LIFE);
    player_set_life(player_two, PLAYER_STARTING_LIFE);
    current_player = player_one;
    layout_group_select_player(layout_group, current_player);
    layout_group_mark_dirty(layout_group);
}

static void select_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
    reset_game();
    vibes_short_pulse();
}

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
    if (round_timer->enabled) {
        round_timer_reset(round_timer);
        reset_game();
        vibes_long_pulse();
    }
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 50, up_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 50, down_click_handler);

    window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 0, 0, true, select_multi_click_handler);

    window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    long time_left = round_timer_tick(round_timer);
    if (time_left <= 0) {
        tick_timer_service_unsubscribe();
        vibes_long_pulse();
    }
}

static void update_layer_group(LayoutGroup *layout_group, Player *player, const char *name) {
    player_set_name(player, name);
    layout_group_update_player(layout_group, player);
}

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
    bool enabled;
    switch (key) {
        case CONFIG_KEY_PLAYER_ONE_NAME:
            update_layer_group(layout_group, layout_group->player_one_layout->player, new_tuple->value->cstring);
            break;
        case CONFIG_KEY_PLAYER_TWO_NAME:
            update_layer_group(layout_group, layout_group->player_two_layout->player, new_tuple->value->cstring);
            break;
        case CONFIG_KEY_ROUND_TIME_ENABLED:
            enabled = new_tuple->value->uint8;
            round_timer_set_enabled(round_timer, enabled);
            if (enabled) tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
            else tick_timer_service_unsubscribe();
            break;
    }
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "app_sync error: %d, %d", dict_error, app_message_error);
}

static ConfigListener *config_listener_create() {
    ConfigListener *config_listener = malloc(sizeof(ConfigListener));
    size_t sync_buffer_size = sizeof(uint8_t) * _CONFIG_LISTENER_SYNC_BUFFER_LEN;
    uint8_t *sync_buffer = malloc(sync_buffer_size);
    config_listener->sync_buffer = sync_buffer;

    app_message_open(app_message_inbox_size_maximum(), 0);
    AppSync *app_sync = malloc(sizeof(AppSync));
    Tuplet initial_values[] = {
        TupletCString(CONFIG_KEY_PLAYER_ONE_NAME, player_one->name),
        TupletCString(CONFIG_KEY_PLAYER_TWO_NAME, player_two->name),
        TupletInteger(CONFIG_KEY_ROUND_TIME_ENABLED, round_timer->enabled),
    };
    app_sync_init(app_sync, sync_buffer, sync_buffer_size,
        initial_values, ARRAY_LENGTH(initial_values),
        sync_changed_handler, sync_error_handler, NULL);
    config_listener->app_sync = app_sync;

    return config_listener;
}

static void config_listener_destroy(ConfigListener *config_listener) {
    app_sync_deinit(config_listener->app_sync);
    free(config_listener->app_sync);
    free(config_listener->sync_buffer);
    free(config_listener);
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

    round_timer = round_timer_create(layout_group);
    config_listener = config_listener_create();
}

static void main_window_unload(Window *window) {
    tick_timer_service_unsubscribe();

    config_listener_destroy(config_listener);
    round_timer_destroy(round_timer);

    action_bar_destroy(action_bar);

    layout_group_destroy(layout_group);

    player_write_to_storage(player_one, PLAYER_ONE_STORAGE_KEY);
    player_write_to_storage(player_two, PLAYER_TWO_STORAGE_KEY);

    player_destroy(player_one);
    player_destroy(player_two);
}

static void focus_handler(bool focus) {
    if (focus && round_timer->enabled) {
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

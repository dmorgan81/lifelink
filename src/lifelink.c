#include <pebble.h>

#include "layout.h"
#include "player.h"
#include "action_bar.h"

static Window *main_window;
static ActionBar *action_bar;
static LayoutGroup *layout_group;

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

static void main_window_load(Window *window) {
#ifdef PBL_PLATFORM_APLITE
    window_set_fullscreen(window, true);
#endif

    player_one = create_player("Player One");
    player_two = create_player("Player Two");
    current_player = player_one;

    action_bar = action_bar_create();
    action_bar_add_to_window(action_bar, window);
    action_bar_set_click_config_provider(action_bar, click_config_provider);

    layout_group = layout_group_create(player_one, player_two);
    layout_group_add_to_window(layout_group, window);
}

static void main_window_unload(Window *window) {
    action_bar_destroy(action_bar);

    layout_group_destroy(layout_group);

    destroy_player(player_one);
    destroy_player(player_two);
}

static void handle_init() {
    main_window = window_create();
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
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

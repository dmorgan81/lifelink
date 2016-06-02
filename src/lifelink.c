#include <pebble.h>
#include "logging.h"

static Window *s_window;
static ActionBarLayer *s_action_bar_layer;
static StatusBarLayer *s_status_bar_layer;

static void window_load(Window *window) {
    log_func();
    Layer *root_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root_layer);

    s_action_bar_layer = action_bar_layer_create();
    action_bar_layer_add_to_window(s_action_bar_layer, window);

    GRect frame = GRect(0, 0, bounds.size.w - ACTION_BAR_WIDTH, STATUS_BAR_LAYER_HEIGHT);
    s_status_bar_layer = status_bar_layer_create();
    layer_set_frame(status_bar_layer_get_layer(s_status_bar_layer), frame);
    layer_add_child(root_layer, status_bar_layer_get_layer(s_status_bar_layer));
}

static void window_unload(Window *window) {
    log_func();
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

#include <pebble.h>
#include "action_bar.h"

static void action_bar_load_icons(ActionBar *action_bar) {
    action_bar->icons[0] = gbitmap_create_with_resource(RESOURCE_ID_LIFE_UP);
    action_bar->icons[1] = gbitmap_create_with_resource(RESOURCE_ID_LIFE_DOWN);
    action_bar->icons[2] = gbitmap_create_with_resource(RESOURCE_ID_SWAP_PLAYER);

    action_bar_layer_set_icon(action_bar->layer, BUTTON_ID_UP, action_bar->icons[0]);
    action_bar_layer_set_icon(action_bar->layer, BUTTON_ID_DOWN, action_bar->icons[1]);
    action_bar_layer_set_icon(action_bar->layer, BUTTON_ID_SELECT, action_bar->icons[2]);
}

ActionBar *action_bar_create() {
    ActionBar *action_bar = malloc(sizeof(ActionBar));
    action_bar->layer = action_bar_layer_create();
    action_bar_load_icons(action_bar);
    return action_bar;
}

void action_bar_destroy(ActionBar *action_bar) {
    int i;
    for (i = 0; i < 3; i++) {
        gbitmap_destroy(action_bar->icons[i]);
    }

    action_bar_layer_destroy(action_bar->layer);
    free(action_bar);
}

void action_bar_add_to_window(ActionBar *action_bar, Window *window) {
    action_bar_layer_add_to_window(action_bar->layer, window);
}

void action_bar_set_click_config_provider(ActionBar *action_bar, ClickConfigProvider click_config_provider) {
    action_bar_layer_set_click_config_provider(action_bar->layer, click_config_provider);
}

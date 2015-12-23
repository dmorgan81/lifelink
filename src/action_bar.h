#pragma once
#include <pebble.h>

typedef struct ActionBar {
    ActionBarLayer *layer;
    GBitmap *icons[3];
} ActionBar;

ActionBar *action_bar_create();
void action_bar_destroy(ActionBar *action_bar);
void action_bar_add_to_window(ActionBar *action_bar, Window *window);
void action_bar_set_click_config_provider(ActionBar *action_bar, ClickConfigProvider click_config_provider);

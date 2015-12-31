#pragma once
#include <pebble.h>
#include "layout.h"

typedef struct ConfigListener {
    AppSync *app_sync;
    uint8_t *sync_buffer;
} ConfigListener;

ConfigListener *config_listener_create(LayoutGroup *layout_group);
void config_listener_destroy(ConfigListener *config_listener);
#include <pebble.h>
#include "config_listener.h"
#include "player.h"

#define _CONFIG_LISTENER_SYNC_BUFFER_LEN 124;

typedef enum {
    CONFIG_KEY_PLAYER_ONE_NAME = 0x1,
    CONFIG_KEY_PLAYER_TWO_NAME = 0x2
} ConfigListenerKeys;

static void update_layer_group(LayoutGroup *layout_group, Player *player, const char *name) {
    player_set_name(player, name);
    layout_group_update_player(layout_group, player);
}

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
    LayoutGroup *layout_group = (LayoutGroup *) context;
    if (key == CONFIG_KEY_PLAYER_ONE_NAME) {
        update_layer_group(layout_group, layout_group->player_one_layout->player, new_tuple->value->cstring);
    } else if (key == CONFIG_KEY_PLAYER_TWO_NAME) {
        update_layer_group(layout_group, layout_group->player_two_layout->player, new_tuple->value->cstring);
    }
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "app_sync error: %d, %d", dict_error, app_message_error);
}

ConfigListener *config_listener_create(LayoutGroup *layout_group) {
    app_message_open(app_message_inbox_size_maximum(), 0);

    Player *player_one = layout_group->player_one_layout->player;
    Player *player_two = layout_group->player_two_layout->player;

    ConfigListener *config_listener = malloc(sizeof(ConfigListener));
    size_t sync_buffer_size = sizeof(uint8_t) * _CONFIG_LISTENER_SYNC_BUFFER_LEN;
    uint8_t *sync_buffer = malloc(sync_buffer_size);
    config_listener->sync_buffer = sync_buffer;

    AppSync *app_sync = malloc(sizeof(AppSync));
    Tuplet initial_values[] = {
        TupletCString(CONFIG_KEY_PLAYER_ONE_NAME, player_one->name),
        TupletCString(CONFIG_KEY_PLAYER_TWO_NAME, player_two->name)
    };
    app_sync_init(app_sync, sync_buffer, sync_buffer_size,
        initial_values, ARRAY_LENGTH(initial_values),
        sync_changed_handler, sync_error_handler, layout_group);
    config_listener->app_sync = app_sync;

    return config_listener;
}

void config_listener_destroy(ConfigListener *config_listener) {
    app_sync_deinit(config_listener->app_sync);
    free(config_listener->app_sync);
    free(config_listener->sync_buffer);
    free(config_listener);
}

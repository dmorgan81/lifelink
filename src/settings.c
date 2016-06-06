#include <pebble.h>
#include "logging.h"
#include "constants.h"
#include "settings.h"

static const uint8_t SETTINGS_VERSION = 1;

typedef struct Node {
    struct Node *next;
    SettingsUpdateListener *listener;
    void *context;
} Node;

static Node *s_head;
static Settings *s_settings;
static AppSync s_sync;
static uint8_t s_sync_buffer[256];

static void settings_save(Settings *this) {
    log_func();
    persist_write_int(PERSIST_KEY_SETTINGS_VERSION, SETTINGS_VERSION);
    persist_write_data(PERSIST_KEY_SETTINGS_DATA, this, sizeof(Settings));
}

static Settings *settings_load(void) {
    log_func();
    Settings *this = malloc(sizeof(Settings));
    int32_t version = persist_read_int(PERSIST_KEY_SETTINGS_VERSION);
    if (version == 0) {
        strncpy(this->player_names[0], "Player One", MAX_NAME_LEN);
        strncpy(this->player_names[1], "Player Two", MAX_NAME_LEN);
        this->round_timer_enabled = false;
        version = 1;
    } else {
        persist_read_data(PERSIST_KEY_SETTINGS_DATA, this, sizeof(Settings));
    }
    settings_save(this);
    return this;
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
    log_func();
    loge("sync error %d %d", dict_error, app_message_error);
}

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
    log_func();
    if (new_tuple == NULL || old_tuple == NULL) {
        return;
    }

    Settings *settings = (Settings *) context;
    switch (key) {
        case AppKeyPlayerOneName:
            strncpy(settings->player_names[0], new_tuple->value->cstring, MAX_NAME_LEN);
            break;
        case AppKeyPlayerTwoName:
            strncpy(settings->player_names[1], new_tuple->value->cstring, MAX_NAME_LEN);
            break;
        case AppKeyRoundTimerEnabled:
            settings->round_timer_enabled = new_tuple->value->uint8;
            break;
    }
    settings_save(settings);

    Node *node = s_head;
    while (node != NULL) {
        (*node->listener)(settings, node->context);
        node = node->next;
    }
}

static void sync_init(Settings *settings) {
    logt("%s", __func__);
    app_message_open(app_message_inbox_size_maximum(), 0);

    Tuplet initial_values[] = {
        TupletCString(AppKeyPlayerOneName, settings->player_names[0]),
        TupletCString(AppKeyPlayerTwoName, settings->player_names[1]),
        TupletInteger(AppKeyRoundTimerEnabled, settings->round_timer_enabled),
    };

    app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
        sync_changed_handler, sync_error_handler, settings);
}

void settings_init(void) {
    log_func();
    s_settings = settings_load();
    sync_init(s_settings);
}

void settings_deinit(void) {
    log_func();
    app_sync_deinit(&s_sync);

    free(s_settings);

    Node *node = s_head;
    while (node != NULL) {
        Node *next = node->next;
        free(node);
        node = next;
    }
}

void settings_add_listener(SettingsUpdateListener *listener, void *context) {
    log_func();
    Node *node = malloc(sizeof(Node));
    node->listener = listener;
    node->context = context;
    node->next = s_head;
    s_head = node;

    (*listener)(s_settings, context);
}

#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include "logging.h"
#include "enamel.h"

static EventHandle *s_event_handle;

static void inbox_received(DictionaryIterator *iterator, void *context) {
    log_func();
}

void sync_init() {
    log_func();

    enamel_init();
    events_app_message_open();

    s_event_handle = events_app_message_subscribe_handlers((EventAppMessageHandlers) {
        .received = inbox_received
    }, NULL);
}

void sync_deinit() {
    log_func();
    events_app_message_unsubscribe(s_event_handle);
    enamel_deinit();
}

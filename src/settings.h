#pragma once
#include "constants.h"

typedef enum {
    AppKeyPlayerOneName = 0,
    AppKeyPlayerTwoName,
    AppKeyRoundTimerEnabled,
} SettingsAppKeys;

typedef struct {
    char player_names[MAX_PLAYERS][MAX_NAME_LEN];
    bool round_timer_enabled;
} Settings;

typedef void (SettingsUpdateListener)(Settings *settings, void *context);

void settings_init(void);
void settings_deinit(void);
void settings_add_listener(SettingsUpdateListener *listener, void *context);

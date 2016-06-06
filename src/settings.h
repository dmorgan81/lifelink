#pragma once
#include "constants.h"

typedef enum {
    AppKeyPlayerOneName = 0,
    AppKeyPlayerTwoName,
    AppKeyRoundTimerEnabled,
    AppKeyRoundLength,
    AppKeyStartingLife,
} SettingsAppKeys;

typedef struct {
    char player_names[MAX_PLAYERS][MAX_NAME_LEN];
    bool round_timer_enabled;
    uint32_t round_length;
    uint8_t starting_life;
} Settings;

typedef void (SettingsUpdateListener)(Settings *settings, void *context);

Settings *settings_init(void);
void settings_deinit(void);
void settings_add_listener(SettingsUpdateListener *listener, void *context);

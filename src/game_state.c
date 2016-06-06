#include <pebble.h>
#include "logging.h"
#include "constants.h"
#include "settings.h"
#include "game_state.h"

GameState *game_state_load(Settings *settings) {
    log_func();
    GameState *this = malloc(sizeof(GameState));
    if (persist_exists(PERSIST_KEY_GAME_STATE)) {
        persist_read_data(PERSIST_KEY_GAME_STATE, this, sizeof(GameState));
    } else {
        for (uint8_t i = 0; i < MAX_PLAYERS; i++) {
            this->life_totals[i] = settings->starting_life;
        }
    }
    return this;
}

void game_state_destroy(GameState *this) {
    log_func();
    this->last_run = time(NULL);
    persist_write_data(PERSIST_KEY_GAME_STATE, this, sizeof(GameState));
    free(this);
}

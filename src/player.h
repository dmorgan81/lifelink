#pragma once
#include <pebble.h>

#define PLAYER_STARTING_LIFE 20
#define PLAYER_MAX_LIFE 999
#define PLAYER_MIN_LIFE -99

#define PLAYER_ONE_STORAGE_KEY 1
#define PLAYER_ONE_NAME_STORAGE_KEY 2
#define PLAYER_TWO_STORAGE_KEY 3
#define PLAYER_TWO_NAME_STORAGE_KEY 4

typedef struct Player {
    int16_t life;
    char *life_text;
    char *name;
} Player;

Player *create_player(char *name);
void destroy_player(Player *player);
void player_set_life(Player *player, int16_t life);
void player_modify_life(Player *player, int16_t life);
bool player_exists_in_storage(uint32_t key);
Player *player_read_from_storage(uint32_t key);
void player_write_to_storage(Player *player, uint32_t key);

#pragma once
#include <pebble.h>

#define PLAYER_STARTING_LIFE 20
#define PLAYER_MAX_LIFE 999
#define PLAYER_MIN_LIFE -99

typedef struct Player {
    int16_t life;
    char *life_text;
    char *name;
} Player;

Player *create_player(char *name);
void destroy_player(Player *player);
void player_set_life(Player *player, int16_t life);
void player_modify_life(Player *player, int16_t life);

#include "player.h"

#define PLAYER_LIFE_BUFFER_SIZE 3

Player *create_player(char *name) {
    Player *player = malloc(sizeof(Player));
    player->name = name;
    player->life_text = malloc(sizeof(char) * PLAYER_LIFE_BUFFER_SIZE);
    player_set_life(player, PLAYER_STARTING_LIFE);
    return player;
}

void destroy_player(Player *player) {
    free(player->life_text);
    free(player);
}

static void player_update_life_text(Player *player) {
    snprintf(player->life_text, sizeof(char) * PLAYER_LIFE_BUFFER_SIZE, "%d", player->life);
}

void player_set_life(Player *player, int16_t life) {
    if (life > PLAYER_MAX_LIFE || life < PLAYER_MIN_LIFE) return;
    player->life = life;
    player_update_life_text(player);
}

void player_modify_life(Player *player, int16_t life) {
    player_set_life(player, player->life + life);
}

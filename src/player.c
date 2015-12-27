#include "player.h"

#define _PLAYER_LIFE_BUFFER_SIZE 4

Player *create_player(char *name) {
    Player *player = malloc(sizeof(Player));

    size_t buf_size = sizeof(char) * PERSIST_STRING_MAX_LENGTH;
    char *buf = malloc(buf_size);
    strncpy(buf, name, buf_size);

    player->name = buf;
    player->life_text = malloc(sizeof(char) * _PLAYER_LIFE_BUFFER_SIZE);
    player_set_life(player, PLAYER_STARTING_LIFE);
    return player;
}

void destroy_player(Player *player) {
    free(player->name);
    free(player->life_text);
    free(player);
}

static void player_update_life_text(Player *player) {
    snprintf(player->life_text, sizeof(char) * _PLAYER_LIFE_BUFFER_SIZE, "%d", player->life);
}

void player_set_life(Player *player, int16_t life) {
    if (life > PLAYER_MAX_LIFE || life < PLAYER_MIN_LIFE) return;
    player->life = life;
    player_update_life_text(player);
}

void player_modify_life(Player *player, int16_t life) {
    player_set_life(player, player->life + life);
}

static uint32_t get_name_key(uint32_t key) {
    return key == PLAYER_ONE_STORAGE_KEY ? PLAYER_ONE_NAME_STORAGE_KEY : PLAYER_TWO_NAME_STORAGE_KEY;
}

bool player_exists_in_storage(uint32_t key) {
    uint32_t name_key = get_name_key(key);
    return persist_exists(key) && persist_exists(name_key);
}

Player *player_read_from_storage(uint32_t key) {
    uint32_t name_key = get_name_key(key);
    char buf[PERSIST_STRING_MAX_LENGTH];

    persist_read_string(name_key, buf,  sizeof(char) * PERSIST_STRING_MAX_LENGTH);
    Player *player = create_player(buf);
    player_set_life(player, persist_read_int(key));
    return player;
}

void player_write_to_storage(Player *player, uint32_t key) {
    uint32_t name_key = get_name_key(key);
    persist_write_int(key, player->life);
    persist_write_string(name_key, player->name);
}

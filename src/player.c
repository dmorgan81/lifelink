#include "player.h"

Player *player_create(char *name) {
    Player *player = malloc(sizeof(Player));

    player_set_name(player, name);
    player_set_life(player, PLAYER_STARTING_LIFE);
    return player;
}

void player_destroy(Player *player) {
    free(player->name);
    free(player);
}

void player_set_life(Player *player, int16_t life) {
    if (life > PLAYER_MAX_LIFE || life < PLAYER_MIN_LIFE) return;
    player->life = life;
}

void player_set_name(Player *player, const char *name) {
    if (player->name) free(player->name);

    size_t buf_size = sizeof(char) * PERSIST_STRING_MAX_LENGTH;
    char *buf = malloc(buf_size);
    strncpy(buf, name, buf_size);
    player->name = buf;
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
    Player *player = player_create(buf);
    player_set_life(player, persist_read_int(key));
    return player;
}

void player_write_to_storage(Player *player, uint32_t key) {
    uint32_t name_key = get_name_key(key);
    persist_write_int(key, player->life);
    persist_write_string(name_key, player->name);
}

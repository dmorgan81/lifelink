#pragma once
#include <pebble.h>
#include "player.h"

#ifndef STATUS_BAR_LAYER_HEIGHT
#define STATUS_BAR_LAYER_HEIGHT PBL_IF_RECT_ELSE(16, 24)
#endif

typedef struct Layout {
    Player *player;
    Layer *group_layer;
    TextLayer *life_layer;
    TextLayer *name_layer;
} Layout;

typedef struct LayoutGroup {
    Layout* player_one_layout;
    Layout* player_two_layout;
    void *extras;
} LayoutGroup;

LayoutGroup *layout_group_create(Player* player_one, Player *player_two);
void layout_group_destroy(LayoutGroup *layout_group);
void layout_group_mark_dirty(LayoutGroup *layout_group);
void layout_group_add_to_window(LayoutGroup *layout_group, Window *window);
void layout_group_select_player(LayoutGroup *layout_group, Player *player);
void layout_group_update_player(LayoutGroup *layout_group, Player *player);

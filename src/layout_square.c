#include <pebble.h>

#include "effect_layer.h"
#include "layout.h"
#include "player.h"

#ifdef PBL_PLATFORM_APLITE

typedef struct Extras {
    EffectLayer *effect_layer;
    PropertyAnimation *animation;
} Extras;

static Layout* layout_group_find_layout(LayoutGroup *layout_group, Player *player) {
    if (layout_group->player_one_layout->player == player)
        return layout_group->player_one_layout;
    else
        return layout_group->player_two_layout;
}

static void layout_update_life_layer(Layout *layout) {
    Player *player = layout->player;
    text_layer_set_text(layout->life_layer, player->life_text);
}

static Layout *layout_create(Player *player) {
    Layout *layout = malloc(sizeof(Layout));
    layout->player = player;
    layout->group_layer = layer_create(GRectZero);

    layout->life_layer = text_layer_create(GRectZero);
    text_layer_set_font(layout->life_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
    text_layer_set_text_alignment(layout->life_layer, GTextAlignmentRight);
    layout_update_life_layer(layout);
    layer_add_child(layout->group_layer, text_layer_get_layer(layout->life_layer));

    layout->name_layer = text_layer_create(GRectZero);
    text_layer_set_text_alignment(layout->name_layer, GTextAlignmentRight);
    text_layer_set_text(layout->name_layer, player->name);
    layer_add_child(layout->group_layer, text_layer_get_layer(layout->name_layer));

    return layout;
}

LayoutGroup *layout_group_create(Player *player_one, Player* player_two) {
    Layout *player_one_layout;
    Layout *player_two_layout;
    EffectLayer *effect_layer;
    LayoutGroup *layout_group = malloc(sizeof(LayoutGroup));
    Extras *extras = malloc(sizeof(Extras));
    layout_group->extras = extras;

    player_one_layout = layout_create(player_one);
    player_two_layout = layout_create(player_two);
    layout_group->player_one_layout = player_one_layout;
    layout_group->player_two_layout = player_two_layout;

    effect_layer = effect_layer_create(GRectZero);
    effect_layer_add_effect(effect_layer, effect_invert, NULL);
    extras->effect_layer = effect_layer;

    return layout_group;
}

static void layout_destroy(Layout *layout) {
    text_layer_destroy(layout->life_layer);
    text_layer_destroy(layout->name_layer);
    layer_destroy(layout->group_layer);

    free(layout);
}

void layout_group_destroy(LayoutGroup *layout_group) {
    layout_destroy(layout_group->player_one_layout);
    layout_destroy(layout_group->player_two_layout);
    effect_layer_destroy(((Extras *) layout_group->extras)->effect_layer);

    free(layout_group->extras);
    free(layout_group);
}

static void layout_mark_dirty(Layout *layout) {
    layout_update_life_layer(layout);
    layer_mark_dirty(layout->group_layer);
}

void layout_group_mark_dirty(LayoutGroup *layout_group) {
    layout_mark_dirty(layout_group->player_one_layout);
    layout_mark_dirty(layout_group->player_two_layout);
}

static void layout_set_frame(Layout *layout, GRect frame) {
    int16_t life_height = frame.size.h * 0.6;

    layer_set_frame(layout->group_layer, frame);
    layer_set_frame(text_layer_get_layer(layout->life_layer), GRect(0, 0, frame.size.w - 5, life_height));
    layer_set_frame(text_layer_get_layer(layout->name_layer), GRect(0, life_height, frame.size.w - 5, frame.size.h - life_height));
}

void layout_group_add_to_window(LayoutGroup *layout_group, Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    int16_t width = bounds.size.w - ACTION_BAR_WIDTH - 5;
    int16_t height = bounds.size.h / 2;

    layout_set_frame(layout_group->player_one_layout, GRect(0, 0, width, height));
    layout_set_frame(layout_group->player_two_layout, GRect(0, height, width, height));

    Extras *extras = (Extras *) layout_group->extras;
    effect_layer_set_frame(extras->effect_layer, layer_get_frame(layout_group->player_one_layout->group_layer));

    layer_add_child(window_layer, layout_group->player_one_layout->group_layer);
    layer_add_child(window_layer, layout_group->player_two_layout->group_layer);
    layer_add_child(window_layer, effect_layer_get_layer(extras->effect_layer));
}

static void animation_stopped(Animation *animation, bool finished, void *data) {
    Extras *extras = (Extras *) data;
    property_animation_destroy(extras->animation);
}

void layout_group_select_player(LayoutGroup *layout_group, Player *player) {
    Layout *layout = layout_group_find_layout(layout_group, player);
    GRect frame = layer_get_frame(layout->group_layer);
    Extras *extras = (Extras *) layout_group->extras;

    extras->animation = property_animation_create_layer_frame(effect_layer_get_layer(extras->effect_layer), NULL, &frame);
    Animation *animation = property_animation_get_animation(extras->animation);
    animation_set_duration(animation, 150);
    animation_set_handlers(animation, (AnimationHandlers) {
        .stopped = (AnimationStoppedHandler) animation_stopped
    }, extras);
    animation_schedule(animation);
}

void layout_group_update_player(LayoutGroup *layout_group, Player *player) {
    Layout *layout = layout_group_find_layout(layout_group, player);
    layout_update_life_layer(layout);
}

#endif
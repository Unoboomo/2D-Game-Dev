#include "simple_logger.h"

#include "gfc_config.h"

#include "gf2d_draw.h"
#include "gf2d_graphics.h"

#include "entity.h"
#include "player.h"
#include "font.h"

#include "widgets.h"
extern Uint32 ms;
extern Uint32 last_ms;
extern int mx, my;
extern int game_done;

void widget_button_draw(Widget* widget, GFC_Vector2D offset);
Uint8 widget_button_update(Widget* widget, GFC_Vector2D offset);
void widget_start_game();
void widget_shop();
void widget_exit_application();

Widget* widget_new()
{
    Widget* widget;
    widget = (Widget*)malloc(sizeof(Widget));
    if (!widget)
    {
        slog("failed to allocate a new window widget");
        return NULL;
    }
    memset(widget, 0, sizeof(Widget));
    return widget;
}

void widget_free(Widget* widget)
{
    if (!widget)return;

    free(widget);
}

void widget_draw(Widget * widget, GFC_Vector2D offset)
{
    if (!widget)
    {
        return;
    }
    if (widget->draw)widget->draw(widget,offset);
}

Uint8 widget_update(Widget * widget, GFC_Vector2D offset)
{
    if (!widget)
    {
        return 0;
    }
    if (widget->update) {
        return widget->update(widget, offset);
    }
    return 0;
}

/**
* WIDGET BUTTON
*/

Widget* widget_button_new_test() {
    Widget* widget;
    widget = widget_new();
    if (!widget) {
        slog("failed to create new widget");
        return NULL;
    }

    gfc_line_cpy(widget->name,"button_test");
    widget->bounds = gfc_rect(100, 100, 130, 40);
    widget->color = GFC_COLOR_BLACK;
    widget->type = WT_Button;

    gfc_line_cpy(widget->display_text, "press me");

    widget->draw = widget_button_draw;
    widget->update = widget_button_update;
    widget->pressed = widget_exit_application;
    return widget;
}

Widget* widget_button_configure_from_json(SJson* json) {
    GFC_Vector4D bounds = { 0 };
    const char* buffer = NULL;
    Widget* widget;

    if ((!json)) {
        return NULL;
    }

    widget = widget_new();
    if (!widget) {
        slog("failed to create new widget");
        return NULL;
    }

    buffer = sj_object_get_string(json, "name");
    if (buffer) {
        gfc_line_cpy(widget->name, buffer);
        buffer = NULL;
    }
    sj_object_get_vector4d(json, "bounds", &bounds);
    widget->bounds = gfc_rect_from_vector4(bounds);

    sj_object_get_color_value(json, "color", &widget->color);

    buffer = sj_object_get_string(json, "display_text");
    if (buffer) {
        gfc_line_cpy(widget->display_text, buffer);
        buffer = NULL;
    }

    widget->type = WT_Button;
    widget->draw = widget_button_draw;
    widget->update = widget_button_update;

    buffer = sj_object_get_string(json, "pressed");
    if (buffer) {
        if (!strcmp(buffer, "exit")) {
            widget->pressed = widget_exit_application;
        }
        else if (!strcmp(buffer, "shop")) {
            widget->pressed = widget_shop;
        }
        else if (!strcmp(buffer, "start")) {
            widget->pressed = widget_start_game;
        }
        else {
            widget->pressed = NULL;
        }
    }
    return widget;
}

void widget_button_draw(Widget* widget, GFC_Vector2D offset) {
    GFC_Rect bounds = { 0 };

    bounds = widget->bounds;
    gfc_vector2d_add(bounds, offset, bounds);

    gf2d_draw_rect_filled(bounds, widget->color);
    if (widget->display_text) {
        font_draw_test(widget->display_text, FS_small, GFC_COLOR_WHITE, gfc_vector2d(bounds.x,bounds.y));
    }
}

Uint8 widget_button_update(Widget* widget, GFC_Vector2D offset) {
    GFC_Rect bounds = { 0 };
    bounds = widget->bounds;
    gfc_vector2d_add(bounds, offset, bounds);

    if (gfc_point_in_rect(gfc_vector2d(mx, my), bounds) && (ms & SDL_BUTTON_LMASK && !(last_ms & SDL_BUTTON_LMASK))) {
        if (widget->pressed) {
            widget->pressed();
        }
        return 1;
    }
    return 0;
}

void widget_exit_application() {
    game_done = 1;
}

void widget_start_game() {
    slog("start");
}
void widget_shop() {
    Entity* player;
    PlayerEntityData* data;

    player = entity_get_player();
    if (!player || !player->data) {
        return;
    }
    data = (PlayerEntityData*)player->data;
    if (data->coin_count >= 20) {
        slog("here's an extra life");
        data->lives_count++;
        data->coin_count -= 20;
    }
    else {
        slog("not enough coins to buy an extra life");
    }
}
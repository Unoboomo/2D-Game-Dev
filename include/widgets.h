#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include "gfc_color.h"
#include "gfc_list.h"
#include "gfc_shape.h"
#include "gfc_text.h"

typedef enum
{
    WS_idle,        /**<normal*/
    WS_disable,     /**<greyed out*/
    WS_highlight,   /**<selected*/
    WS_hidden,
    wsdsS_active       /**<pressed or whatever*/
}WidgetState;

typedef enum
{
    WUR_None,
    WUR_Handled
}WidgetUpdateReturn;

typedef enum
{
    WT_List,
    WT_Text,
    WT_Sprite,
    WT_Button,
    WT_Entry
}WidgetTypes;

typedef struct Widget_S
{
    GFC_TextLine name;              //name of the widget
    GFC_Rect bounds;                //draw bounds of the widget
    GFC_Color color;                //color for the widget

    WidgetTypes type;               //which type of widget this is

    GFC_TextLine display_text;

    void (*draw)        (struct Widget_S* widget, GFC_Vector2D offset); /**<draw function, offset comes from draw position of window*/
    Uint8 (*update)     (struct Widget_S* widget, GFC_Vector2D offset); /**<function called for updates  returns alist of all widgets updated with input*/
    void (*pressed)     (void);
}Widget;

/**
 * @brief allocate and initialize a new widget
 * @return NULL on error or a new widget otherwise;
 */
Widget* widget_new();

/**
 * @brief free an widget
 * @param widget the widget to free
 */
void widget_free(Widget* widget);

/**
 * @brief draw an widget
 * @param widget the widget to draw
 * @param offset comes from parent window position
 */
void widget_draw(Widget* widget, GFC_Vector2D offset);

/**
 * @brief update an widget
 * @param widget the widget to update
 * @param offset comes from parent window position
 * @return 1 if the widget was updated, zero otherwise
 */
Uint8 widget_update(Widget* widget, GFC_Vector2D offset);

Widget* widget_button_configure_from_json(SJson* json);

Widget* widget_button_new_test();

#endif
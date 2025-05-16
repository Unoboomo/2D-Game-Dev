#ifndef __WINDOWS_H__
#define __WINDOWS_H__

#include "gfc_color.h"
#include "gfc_list.h"
#include "gfc_shape.h"
#include "gfc_text.h"

typedef struct Widget_S Widget;

typedef struct Window_S
{
    int _inuse;             /**<do not touch*/
    int visible;

    GFC_Rect dimensions;        /**<where on the screen*/

    GFC_List* widgets;         /**<all the widgets of the window*/

    GFC_Color color;         /**<color to draw the window with*/
}Window;


/**
 * @brief initialize the window system
 * @param max the limit of active windows
 */
void windows_init(int max);

/**
 * @brief draw all active windows
 */
void window_system_draw_all();

/**
 * @brief update all active windows
 */
void windows_system_update_all();

/**
 * @brief get a new initialized window
 * @return NULL on error or a window pointer
 */
Window* window_new();

/**
 * @brief free a window no longer in use
 * @param win the window to free
 */
void window_free(Window* win);

/**
 * @brief draw a window to the screen.
 * @param win the window to draw
 */
void window_draw(Window* win);

/**
 * @brief update a window and all of its elements
 * @param win the window to update
 */
void window_update(Window* win);

Window* window_test();

/**
 * @brief load a window config from file
 * @param window the window to load the info to
 * @param filename the filename of the config file
 */
void window_configure_from_file(Window* window, char* filename);

/**
 * @brief load a window config from json
 * @param window the window to load the info to
 * @param json the json holding the config info
 */
void window_configure_from_json(Window* window, SJson* json);

/**
 * @brief add a gui element to the window
 * @param win the window to add an element to
 * @param w the element to add
 */
void window_add_widget(Window* win, Widget* widget);

/**
 * @brief get a new initialized window loaded from a file
 * @param filename the filename of the config file for the window
 */
Window* window_new_from_file(char* filename);
#endif
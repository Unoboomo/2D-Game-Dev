#include "simple_logger.h"

#include "gfc_config.h"

#include "gf2d_draw.h"
#include "gf2d_graphics.h"

#include "windows.h"

typedef struct
{
    
    Window* window_list;    /**<*/
    int window_max;         /**<how many windows can exist at once*/
}WindowSystem;

static WindowSystem window_system = { 0 };

void windows_close() {
    int i;
    for (i = 0; i < window_system.window_max; i++) {
        if (window_system.window_list[i]._inuse) {
            window_free(&window_system.window_list[i]);
        }
    }
    if (window_system.window_list) {
        free(window_system.window_list);
        window_system.window_list = NULL;
    }
    memset(&window_system, 0, sizeof(WindowSystem));

    slog("window system closed");
}

void windows_init(int max) {
    if (window_system.window_list) {
        slog("cannot initialize multiple window systems");
        return;
    }
    if (max <= 0)
    {
        slog("cannot initilize window system for 0 windows");
        return;
    }
    window_system.window_max = max;
    window_system.window_list = gfc_allocate_array(sizeof(Window), max);
    if (!window_system.window_list)
    {
        slog("failed to allocate memory for window system");
        return;
    }
    slog("window system initilized");
    atexit(windows_close);
}

void window_system_draw_all() {
    int i;
    for (i = 0; i < window_system.window_max; i++) {
        if (window_system.window_list[i]._inuse && window_system.window_list[i].visible) {
            window_draw(&window_system.window_list[i]);
        }
    }
}

Window* window_new() {
    int i;
    for (i = 0; i < window_system.window_max; i++)
    {

        if (!window_system.window_list[i]._inuse)
        {
            window_system.window_list[i]._inuse = 1;
            window_system.window_list[i].visible = 0;
            window_system.window_list[i].widgets = gfc_list_new();
            return &window_system.window_list[i];
        }
    }
    return NULL;
}

void window_free(Window* win) {
    int i, count;

    if (!win)return;

    if (win->free_data != NULL)
    {
        win->free_data(win);
    }

    count = gfc_list_get_count(win->widgets);

    for (i = 0; i < count; i++) {
        //free widget
    }
    gfc_list_delete(win->widgets);

    memset(win, 0, sizeof(Window));
}

void window_draw(Window* win) {
    int count, i;
    GFC_Vector2D offset;
    if (!win || !win->visible) {
        return;
    }

    gf2d_draw_rect_filled(win->dimensions, win->color);

    offset.x = win->dimensions.x;
    offset.y = win->dimensions.y;

    count = gfc_list_get_count(win->widgets);
    for (i = 0; i < count; i++)
    {
        //draw widgets
    }
}

Window* window_test() {
    GFC_Vector2D resolution;
    Window* window;

    window = window_new();
    if (!window) {
        slog("failed to create new window");
        return NULL;
    }

    resolution = gf2d_graphics_get_resolution();
    window->dimensions = gfc_rect(100, 100, resolution.x - 200, resolution.y - 200);
    window->color = GFC_COLOR_GREY;
    window->color.a = 240;
    window->visible = 0;
    return window;
}


void window_configure_from_file(Window* window, char* filename) {
    SJson* json;
    if (!window) {
        return;
    }
    json = sj_load(filename);
    if (!json) {
        slog("json file does not exist");
        return;
    }
    window_configure_from_json(window, json);
    sj_free(json);
}


void window_configure_from_json(Window* window, SJson* json) {
    int i, count;
    SJson* elements, * value;
    const char* buffer;
    GFC_Vector4D bounds = { 0 };

    if ((!window) || (!json)) {
        return;
    }

    sj_object_get_color_value(json, "color", &window->color);

    sj_object_get_vector4d(json, "dimensions", &bounds);
    window->dimensions = gfc_rect_from_vector4(bounds);

    window->visible = 0;


    /**
    * ADD WINDOW WIDGETS
    elements = sj_object_get_value(json, "elements");
    count = sj_array_get_count(elements);
    for (i = 0; i < count; i++)
    {
        value = sj_array_get_nth(elements, i);
        if (!value)continue;
        gf2d_window_add_element(win, gf2d_element_load_from_config(value, NULL, win));
    }
    */
}

Window* window_new_from_file(char* filename) {
    Window* window;

    window = window_new();
    if (!window) {
        slog("failed to create new window");
        return NULL;
    }
    window_configure_from_file(window, filename);

    return window;
}

#ifndef __FONT_H__
#define __FONT_H__

#include <SDL_ttf.h>

#include "gfc_color.h"
#include "gfc_text.h"

typedef enum {
	FS_small,
	FS_medium,
	FS_large,
	FS_MAX
}FontStyles;

/**
* @brief initialize the font system
* @note auto closes on exit
*/
void font_init();

/**
* @brief render text to the screen
* @param text what to render
* @param style what style of font to render with
* @param color the color of the text
* @param position where on the screen to draw to 
*/
void font_draw_test(const char* text, FontStyles style, GFC_Color color, GFC_Vector2D position);

/**
* @brief periodically call this to cleanup any internal cache
*/
void font_cleanup();

#endif
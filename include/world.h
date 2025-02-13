#ifndef __WORLD_H__
#define __WORLD_H__

#include "gf2d_sprite.h"

typedef struct
{
	Sprite* background;	//background image for the world
	Sprite* tileset;	//sprite containing tiles for the world
	Uint8* tile_map;		//the tiles that make up the world
	Uint32 tile_height;	//how many tiles tall the map is
	Uint32 tile_width;	//how many tiles wide the map is
}World;

/**
* @breif test to see if world is working
* @reurn NULL on error, or a blank world
*/
World* world_test_new();

/**
* @breif allocates a new empty world
* @param width the number of tiles wide the world is
* @param height the number of tiles high the world is
* @reurn NULL on error, or a blank world
*/
World* world_new(Uint32 width, Uint32 height);

/**
* @brief free a previously allocated world
* @param *self a pointer to the world to free
*/
void world_free(World* world);

/**
* @brief draws an world to the screen
* @param *self a pointer to the world to draw
*/
void world_draw(World* world);
#endif
#ifndef __WORLD_H__
#define __WORLD_H__

#include "gf2d_sprite.h"
#include "gf2d_graphics.h"

typedef struct
{
	Sprite* background;	//background image for the world
	Sprite* tile_layer;	//prerendered tile layer
	Sprite* tileset;	//sprite containing tiles for the world
	Uint8* tile_map;	//the tiles that make up the world
	Uint32 tile_height;	//how many tiles tall the map is
	Uint32 tile_width;	//how many tiles wide the map is
}World;

/**
* @brief draws the tiles of the world to a surface, to reduce draw calls
* @param *self a pointer to the world to draw
*/
void world_tile_layer_build(World* world);

/**
* @breif loading world from a config file
* @param filename the name of the world file to load
* @return NULL on error, a usable world otherwise
*/
World* world_load(const char* filename);

/**
* @breif test to see if world is working
* @return NULL on error, or a blank world
*/
World* world_test_new();

/**
* @breif allocates a new empty world
* @param width the number of tiles wide the world is
* @param height the number of tiles high the world is
* @return NULL on error, or a blank world
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

/**
* @brief set the camera bounds to the world size
* @param the world whose bounds we set the camera's to
*/
void world_setup_camera(World* world);

#endif
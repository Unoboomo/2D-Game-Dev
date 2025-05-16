#ifndef __WORLD_H__
#define __WORLD_H__

#include "gf2d_draw.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

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
* @brief spawns a tile to the world
* @param position the position to spawn the tile
* @param tile_value what type of tile to spawn
*/
void world_spawn_tile(GFC_Vector2D position, int tile_value);

/**
* @breif saves world to a config file
* @param a pointer to the world to save
* @param filename the name of the world file to save to
*/
void world_save(World* world, const char *filename);

/**
* @breif loading world from a config file
* @param filename the name of the world file to load
* @return NULL on error, a usable world otherwise
*/
World* world_load(const char* filename);

/**
* @breif returns the active world
* @return NULL on no active world
*/
World* world_get_active();

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
* @breif gets the value of the tile in a tile map
* @param world the world whos tilemap to search
* @param position the position in the tilemap of the tile
* @return the value of the tile (and 0 on Error)
*/
Uint8 world_get_tile_at(World* world, GFC_Vector2D position);

/**
* @breif sets the value of the tile in a tile map
* @param world the world whos tilemap to search
* @param position the position in the tilemap of the tile
* @param tile_value the value of the tile 
*/
void world_set_tile_at(World* world, GFC_Vector2D position, int tile_value);

/**
* @brief draws an world background to the screen
* @param *self a pointer to the world to draw
*/
void world_draw_background(World* world);

/**
* @brief draws an world tile layer to the screen
* @param *self a pointer to the world to draw
*/
void world_draw_tile_layer(World* world);

/**
* @brief set the camera bounds to the world size
* @param the world whose bounds we set the camera's to
*/
void world_setup_camera(World* world);

/**
* @brief checks to see a rect collides with the world anywhere
* @param world the world to check
* @param bounds the rect being checked (in worldspace)
* @return 1 if the two are colliding, 0 if not
*/
Uint8 world_test_collision_rect(World* world, GFC_Rect bounds);

void world_switch_to_next();

#endif
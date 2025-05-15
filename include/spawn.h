#ifndef __SPAWN_H__
#define __SPAWN_H__

#include "gfc_config.h"
#include "gfc_config_def.h"

#include "gfc_vector.h"
#include "gfc_shape.h"

#include "entity.h"


typedef struct SpawnLinks_s
{
	const char* name;
	Entity* (*spawn)(GFC_Vector2D position); //pointer to a entity_new function		
}SpawnLinks;

/**
* @brief spawns entities from json into the current world
* @param json the json to get entity data from
*/
void spawn_entities_from_json(SJson* json);

/**
* @brief spawns entities to world at 
* @param spawn_position the world position to spawn the entity to
* @param entity_name the name of the entity to spawn
* @param contents (OPTIONAL, NULL if none) the name of the contents of the entity
*/
void spawn_entity_to_world(GFC_Vector2D spawn_position, const char* entity_name, const char* contents);

/**
* @brief wrapper for spawn_entity_to_world that locks all positions to the tilegrid
* @param spawn_position the world position to spawn the entity to
* @param entity_name the name of the entity to spawn
* @param contents (OPTIONAL, NULL if none) the name of the contents of the entity
*/
void spawn_entity_to_world_gridlock(GFC_Vector2D spawn_position, const char* entity_name, const char* contents);

/**
* @brief finds the world bounds from initial bounds and position
* @param spawn_position the world position
* @param bounds a rect containing the bounds
* @return returns a rect containing the world_bounds
*/
GFC_Rect get_world_bounds_position(GFC_Vector2D spawn_position, GFC_Rect bounds);

/**
* @brief spawns entities to world at
* @param spawn_position the world position to spawn the entity to
* @param bounds the bounds of the entity to spawn
* @return returns 0 if there are no collisions against the world, returns 1 if there are
*/
Uint8 spawn_check_bounds_against_world(GFC_Rect bounds);

/**
* @brief spawns entities to world at
* @param spawn_position the world position to spawn the entity to
* @param bounds the bounds of the entity to spawn
* @return returns 0 if there are no collisions against the world, returns 1 if there are
*/
Uint8 spawn_check_bounds_against_entities(GFC_Rect bounds);
#endif
#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_text.h"
#include "gfc_vector.h"
#include "gfc_shape.h"

#include "gf2d_sprite.h"

#include "physics.h"

typedef enum { //entities of the same type do not collide with each other
	ETT_none,
	ETT_player,
	ETT_monster,
	ETT_item,
	ETT_world,
	ETT_MAX
}EntityTeamType;

typedef enum { 
	ECL_none = 0,
	ECL_World = 1,
	ECL_Entity = 2,
	ECL_Item = 4,
	ECL_ALL = 7
}EntityCollisionLayers;

typedef struct Entity_s
{
	Uint8					_inuse;								//memory management flag

	Uint8					player;								//1 if player, 0 otherwise

	GFC_TextLine			name;								//name of the entity for debugging
	EntityTeamType			team;								//which team this entity is on
	EntityCollisionLayers	layer;								//bitmask for layers, only same layer can collide	

	Sprite					*sprite;							//graphical representation of the entity
	float					frame;								//for drawing the sprite

	Physics_Object			*physics;							//a struct holding all the physics variables for the entity

	void					(*think)(struct Entity_s* self);														//pointer to a think function
	void					(*update)(struct Entity_s* self);														//pointer to a update function (all entities can think based on the same gamestate, then update all)
	void					(*touch)(struct Entity_s* self, struct Entity_s* other, GFC_Vector2D collision_side);	//pointer to a touch function 
	void					(*free)(struct Entity_s* self);															//pointer to a free function

	void					*data;								//the storage for unique entity type data, eg. (gclient, monsterdata, itemdata, etc.)			
}Entity;


/**
* @brief initialize the entity manager
* @param max upper limit for how many entities can exist at once
*/
void entity_system_init(Uint32 max);

/**
* @brief free all entities in the manager
* does not close the entity manager
*/
void entity_system_free_all();

/**
* @brief Draw all entities in the manager
*/
void entity_system_draw_all();

/**
* @brief Draw all entities in a list
* @param entities a GFC_List of entity pointers
*/
void entity_system_draw_list();

/**
* @brief Run all entity think functions
*/
void entity_system_think_all();

/**
* @brief Run all entity update functions
*/
void entity_system_update_all();

/**
* @breif allocates a new empty entity
* @reurn NULL on error, or a blank entity
*/
Entity* entity_new();

/**
* @brief free a previously created entity
* @param *self a pointer to the entity to free
*/
void entity_free(Entity* self);

/**
* @brief configure an entity based on provided filename that contains a config
* @param self the entity to config
* @param filename the filename of the json providing entity information
*/
void entity_configure_from_file(Entity* self, const char* filename);

/**
* @brief configure an entity based on provided json config
* @param self the entity to config
* @param json the json providing entity information
*/
void entity_configure(Entity* self, SJson* json);

/**
* @brief updates the position of an entity from its velocity and ensures it is still in the window
* @param *self a pointer to the entity to update the position of
*/
void entity_update_position(Entity* self);

/**
* @brief checks if the entity's team matches with another team
* @param *self a pointer to the entity to check the team of
* @param team the team to check against
* @return the matching bitmask (will be 0 if no overlap, a non-zero otherwise)
*/
Uint8 entity_check_team(Entity* self, EntityTeamType team);

/**
* @brief checks the entity layers collide with the other layers
* @param *self a pointer to the entity to check the layers of
* @param layer the layers to check against
* @return the matching bitmask (will be 0 if no overlap, a non-zero otherwise)
*/
Uint8 entity_check_layer(Entity* self, EntityCollisionLayers layer);

/**
* @brief updates an entity's layer bitmask with a new layer
* @param *self a pointer to the entity to update the layers of
* @param layer the layer(s) to add 
*/
void entity_set_collision_layer(Entity* self, EntityCollisionLayers layer);

/**
* @brief updates an entity's layer bitmask by removing a layer
* @param *self a pointer to the entity to update the layers of
* @param layer the layer(s) to remove
*/
void entity_remove_collision_layer(Entity* self, EntityCollisionLayers layer);

/**
* @brief ensures an entity is still in the world/window/camera bounds
* @param *self a pointer to the entity to check the bounds of
*/
void entity_check_world_bounds(Entity* self);

/**
* @brief checks an entity's prospective position for collision against other entities,
* @param *self a pointer to the entity to check with
*/
void entity_collide_all(Entity* self);

/**
* @brief gets a list of all entitys with a specific entity_name
* @param the entity name to look for
* @return a List of pointers to entities
* @note the gfc_list returned must be cleaned up with gfc_list_delete
*/
GFC_List* entity_find_by_name(const char* entity_name);

#endif
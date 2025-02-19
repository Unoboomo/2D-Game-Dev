#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_text.h"
#include "gfc_vector.h"
#include "gfc_shape.h"

#include "gf2d_sprite.h"

#include "physics.h"

//create team struct

typedef struct Entity_s
{
	Uint8			_inuse;			//memory management flag
	GFC_TextLine	name;			//name of the entity for debugging
	Sprite			*sprite;		//graphical representation of the entity
	float			frame;			//for drawing the sprite
	Uint8			team;			//which entities can collide with other entities. If same team, cannot collide

	Physics_Object	*physics;		//a struct holding all the physics variables for the entity

	void			(*think)(struct Entity_s* self);	//pointer to a think function
	void			(*update)(struct Entity_s* self);	//pointer to a update function (all entities can think based on the same gamestate, then update all)
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

#endif
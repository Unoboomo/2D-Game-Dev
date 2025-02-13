#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "gfc_config.h"

#define FGRAV ((GFC_Vector2D) {0, 0.15})

typedef struct Physics_Object_s
{
	//Physics!!! YAY!
	GFC_Vector2D	position;				//the position of the entity in space
	GFC_Vector2D	velocity;				//how fast the entity is moving
	float			horizontal_velocity_cap;//the max speed an entity can move horizontally
	Uint8			running;				//if 1, doubles the foreward velocity cap
	float			downward_velocity_cap;	//the max speed an entity can move vertically
	GFC_Vector2D	acceleration;			//how fast the velocity changes 
	int				dir_face;				//what direction the entity is facing, right is 1, left is -1

	/* Physics stuff ripped from quake 2
	float				speed, accel, decel;
	GFC_Vector2D		movedir;
	GFC_Vector2D		pos1, pos2;

	GFC_Vector2D		velocity;
	GFC_Vector2D		avelocity;
	int					mass;
	float				air_finished;
	float				gravity;
	*/
}Physics_Object;

/**
* @breif allocates a new empty physics object
* @reurn NULL on error, or a blank physics object
*/
Physics_Object* physics_obj_new();

/**
* @brief free a previously created physics object
* @param *self a pointer to the physics object to free
*/
void physics_obj_free(Physics_Object* self);


/**
* @brief configure an physics object based on provided filename that contains a config
* @param self the physics object to config
* @param filename the filename of the json providing physics object information
*/
void physics_obj_configure_from_file(Physics_Object* self, const char* filename);

/**
* @brief configure an physics object based on provided json config
* @param self the physics object to config
* @param json the json providing physics object information
*/
void physics_obj_configure(Physics_Object* self);


/**
* @brief calculate a change in position based on position and velocity
* @param self the physics object to update
* @return NULL on error, or a pointer to the spawned player entity
*/
void physics_update(Physics_Object* self);


#endif
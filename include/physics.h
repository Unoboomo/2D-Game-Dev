#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "gfc_vector.h"
#include "gfc_config.h"
#include "gfc_shape.h"

#define FGRAV ((GFC_Vector2D) {(float) 0, (float) 0.7})
#define ZEROGRAV ((GFC_Vector2D) {(float) 0, (float) 0})


typedef struct Physics_Object_s
{
	//Physics!!! YAY!
	GFC_Vector2D	position;				//the position of the entity in space (the entity is centered on this point)
	GFC_Vector2D	velocity;				//how fast the entity is moving
	GFC_Vector2D	acceleration;			//how fast the velocity changes 

	GFC_Vector2D	gravity;

	float			horizontal_velocity_cap;//the max speed an entity can move horizontally
	float			override_horizontal_velocity_cap;	//override horizontal velocity cap with this value

	float			horizontal_deceleration;//the acceleration to slow an entity moving past their horizontal velocity cap by

	float			downward_velocity_cap;	//the max speed an entity can move vertically
	float			override_downward_velocity_cap;	//override downward velocity cap with this value


	Uint8			running;				//if 1, doubles the foreward velocity cap
	Uint8			grounded;				//if the physics object is touching the ground
	int				x_world_collision;		//if the physics object colliding with the world on the x axis, positive for right side, negative for left side

	int				dir_face;				//what direction the entity is facing, right is 1, left is -1

	GFC_Vector2D	center;					//the center of the entity's bounding box
	GFC_Rect		bounds;					//the bounds for collision of an entity
	//GFC_Rect		bounds_position;		//the position in space of the bounds of an entity



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
void physics_obj_configure(Physics_Object* self, SJson* json);

/**
* @brief calculates the position of a physics objects bounds in space
* @param self the physics object to get bounds position of
* @return a GFC_Rect containing the bounds position
*/
GFC_Rect physics_obj_get_world_bounds_position(Physics_Object* self);

/**
* @brief calculates changes in position and velocity
* @param self the physics object to update
* @return NULL on error, or a pointer to the spawned player entity
*/
void physics_update(Physics_Object* self);

/**
* @brief checks to see if two physics objects are colliding
* @param self the first physics object being checked
* @param other the other physics object being checked
* @return 1 if the objects are colliding, 0 if not
*/
Uint8 physics_obj_collision_check(Physics_Object* self, Physics_Object* other);
#endif
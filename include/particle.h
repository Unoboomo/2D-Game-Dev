#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "simple_json.h"

#include "gfc_color.h"
#include "gfc_shape.h"
#include "gfc_text.h"
#include "gfc_vector.h"

#include "gf2d_sprite.h"

typedef enum {
	PT_Point,
	PT_Shape,
	PT_Sprite,
	PT_MAX
}ParticleType;

typedef struct {
	Uint8			_inuse;
	ParticleType	p_type;			//what type the particle is
	GFC_TextLine	name;			//name of the particle for debugging

	Uint32			ttl;			//time to live in frames

	GFC_Vector2D	position;		//the position of the particle in space
	GFC_Vector2D	velocity;		//how fast the particle is moving
	GFC_Vector2D	acceleration;	//how fast the velocity changes 
	
	GFC_Color		color;			//the color of the particle
	GFC_Color		color_velocity;	//change in color over time		

	GFC_Shape		shape;

	Sprite			*sprite;
	float			frame;			
	float			frame_rate;
	int				frame_start;
	int				frame_end;		//last frame of animation
	Uint8			loops;			//if true, loop, else stop on last frame
}Particle;

/**
* @brief sets up the particle manager
* @param max_particles the maximum number of particles that can exist at once
*/
void particle_system_init(Uint32 max_particles);

/**
* @brief clears all active particles
*/
void particle_system_clear();

/**
* @brief updates and draws all active particles in the manager
*/
void particle_system_draw();

/**
* @brief get a new particle
* @return a pointer to the new particle
*/
Particle* particle_new();
 
/**
* @brief free a particle
* @param particle the particle to free
*/
void particle_free(Particle* particle);

/**
* @brief configure particles based on provided filename that contains a config
* @param name the name of the particle to create
* @param count how many particles to create
* @param position where to create the particles
* @param direction what direction the particles will travel in
* @param acceleration what the acceleration of the particle will be
*/
void particles_from_def(const char* name, Uint32 count, GFC_Vector2D position, GFC_Vector2D direction, GFC_Vector2D acceleration);

/**
* @brief configure an particle based on provided json config
* @param json the json providing particle information
* @param position where to create the particles
* @param direction what direction the particles will travel in
* @param acceleration what the acceleration of the particle will be
* @return returns a pointer to the particle
*/
Particle* particle_from_json(SJson* json, GFC_Vector2D position, GFC_Vector2D direction, GFC_Vector2D acceleration);
#endif
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"


#define GROUND_ACCELERATION 0.04
#define MIDAIR_ACCELERATION 0.04

#define ICE_ACCELERATION_MODIFIER 0.5

#define WALL_SLIDE_SPEED 1

#define JUMP_VELOCITY -9
#define WALL_JUMP_VELOCITY 4

#define JUMP_BUFFER 12
#define WALL_JUMP_BUFFER 12

#define GROUND_BOUND_BOOST -12
#define GROUND_POUND_DELAY 13
#define GROUND_POUND_VELOCITY_OVERRIDE 12
#define GROUND_POUND_RECOVERY 15
#define GROUND_POUND_RECOVERY_JUMP_BOOST 1.2

#define CROUCH_MAX_VELOCITY 0.5
#define CROUCH_JUMP_VELOCITY -6

#define LONG_JUMP_THRESHOLD 3.5
#define LONG_JUMP_VELOCITY 6

#define MAX_LIVES 999



typedef struct PlayerEntityData_s
{
	Uint8	jump_buffer;			//gives the player a #frame jump buffer so they are not frame perfect
	Uint8	wall_jump_buffer;		//gives the player a #frame wall jump buffer so they are not frame perfect
	int		last_x_collision_dir;	// holds the last world collision x direction, + for right, - for left

	Uint8	ground_pound_delay;		//freezes the player's movement to perform the ground pound
	Uint8	ground_pounding;		//is the player ground pounding?
	Uint8	ground_pound_recovery;	//freezes the player's movement while recovering from the ground

	Uint8	crouching;				//is the player crouching?

	int		coin_count;
	int		lives_count;
}PlayerEntityData;
/**
* @brief spawn a new player entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned player entity
*/
Entity* player_new(GFC_Vector2D position);


#endif
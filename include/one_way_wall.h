#ifndef __ONE_WAY_WALL_H__
#define __ONE_WAY_WALL_H__

#include "entity.h"

/**
* @brief spawn a new one_way_wall entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned one_way_wall entity
*/
Entity* one_way_wall_new(GFC_Vector2D position);


#endif
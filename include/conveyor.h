#ifndef __CONVEYOR_H__
#define __CONVEYOR_H__

#include "entity.h"

/**
* @brief spawn a new conveyor entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned conveyor entity
*/
Entity* conveyor_new(GFC_Vector2D position);


#endif
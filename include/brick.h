#ifndef __BRICK_H__
#define __BRICK_H__

#include "entity.h"

/**
* @brief spawn a new brick entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned brick entity
*/
Entity* brick_new(GFC_Vector2D position);


#endif
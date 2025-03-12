#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "entity.h"

/**
* @brief spawn a new platform entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned platform entity
*/
Entity* platform_new(GFC_Vector2D position);


#endif
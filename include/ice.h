#ifndef __ICE_H__
#define __ICE_H__

#include "entity.h"

/**
* @brief spawn a new ice entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned ice entity
*/
Entity* ice_new(GFC_Vector2D position);


#endif
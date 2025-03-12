#ifndef __COIN_H__
#define __COIN_H__

#include "entity.h"

/**
* @brief spawn a new coin entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned coin entity
*/
Entity* coin_new(GFC_Vector2D position);


#endif
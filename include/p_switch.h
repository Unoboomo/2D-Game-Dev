#ifndef __P_SWITCH_H__
#define __P_SWITCH_H__

#include "entity.h"

/**
* @brief spawn a new p_switch entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned p_switch entity
*/
Entity* p_switch_new(GFC_Vector2D position);


#endif
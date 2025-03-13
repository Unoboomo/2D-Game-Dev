#ifndef __TRAMPOLINE_H__
#define __TRAMPOLINE_H__

#include "entity.h"

/**
* @brief spawn a new trampoline entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned trampoline entity
*/
Entity* trampoline_new(GFC_Vector2D position);


#endif
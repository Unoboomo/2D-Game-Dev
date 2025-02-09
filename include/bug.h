#ifndef __BUG_H__
#define __BUG_H__

#include "entity.h"

/**
* @brief spawn a new bug entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned bug entity
*/
Entity* bug_new(GFC_Vector2D position);


#endif
#ifndef __BULLET_BILL_H__
#define __BULLET_BILL_H__

#include "entity.h"

/**
* @brief spawn a new bullet_bill entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned bullet_bill entity
*/
Entity* bullet_bill_new(GFC_Vector2D position);


#endif
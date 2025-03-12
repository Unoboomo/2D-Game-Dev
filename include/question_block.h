#ifndef __QUESTION_BLOCK_H__
#define __QUESTION_BLOCK__

#include "entity.h"

/**
* @brief spawn a new question_block entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned question_block entity
*/
Entity* question_block_new(GFC_Vector2D position);

/**
* @brief spawn a new invisible_question_block entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned invisible_question_block entity
*/
Entity* invisible_question_block_new(GFC_Vector2D position);

/**
* @brief spawn a new empty_question_block entity
* @param position where to spawn it
* @return NULL on error, or a pointer to the spawned empty_question_block entity
*/
Entity* empty_question_block_new(GFC_Vector2D position);
#endif
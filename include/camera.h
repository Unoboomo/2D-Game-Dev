#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "gfc_vector.h"
#include "gfc_shape.h"

/**
* @breif get the camera's position in world space
* @return the position
*/
GFC_Vector2D camera_get_position();

/**
* @breif set the camera's position in worldspace
* @param the position to set to
*/
void camera_set_position(GFC_Vector2D position);

/**
* @breif set the camera's size (usually the screen size
* @param the size to set to
*/
void camera_set_size(GFC_Vector2D size);

/**
* @breif get the camera's bounds in world space
* @return the bounds
*/
GFC_Rect camera_get_bounds();

/**
* @breif set the camera's bounds in worldspace
* @param the bounds to set to
*/
void camera_set_bounds(GFC_Rect bounds);

/**
* @breif enable the camera to snap to world bounds
* @param true of false
*/
void camera_enable_binding(Uint8 bind_camera);
	
/**
* @breif get the offset to draw things relative to the camera
* @return the offset
*/
GFC_Vector2D camera_get_offset();

/**
* @breif snap the camera to world bounds
*/
void camera_apply_bounds();

/**
* @breif center the camera on a target
* @param the target
*/
void camera_center_on(GFC_Vector2D target);

#endif
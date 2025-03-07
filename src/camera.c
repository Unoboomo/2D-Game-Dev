#include "simple_logger.h"

#include "camera.h"

typedef struct {
	GFC_Vector2D	position;
	GFC_Vector2D	size;		//width and height of the screen
	GFC_Rect		bounds;
	Uint8			bind_camera;
}Camera;

static Camera _camera = { 0 };

GFC_Vector2D camera_get_position() {
	return _camera.position;
}

void camera_set_position(GFC_Vector2D position) {
	gfc_vector2d_copy(_camera.position, position);
	if (_camera.bind_camera) {
		camera_apply_bounds();
	}
}

void camera_set_size(GFC_Vector2D size) {
	gfc_vector2d_copy(_camera.size, size);
}

GFC_Rect camera_get_bounds() {
	return _camera.bounds;
}

void camera_set_bounds(GFC_Rect bounds) {
	gfc_rect_copy(_camera.bounds, bounds);
}

void camera_enable_binding(Uint8 bind_camera) {
	_camera.bind_camera = bind_camera;
}

GFC_Vector2D camera_get_offset() {
	return gfc_vector2d(-_camera.position.x, -_camera.position.y);
}

void camera_apply_bounds() {
	if ((_camera.position.x + _camera.size.x) > (_camera.bounds.x + _camera.bounds.w)) { //if right side of camera is past the right edge of the world, snap to right edge
		_camera.position.x = _camera.bounds.x + _camera.bounds.w - _camera.size.x;
	 }

	if (_camera.position.x < _camera.bounds.x) { //^^^ but left edge
		_camera.position.x = _camera.bounds.x;
	}

	if ((_camera.position.y + _camera.size.y) > (_camera.bounds.y + _camera.bounds.h)) { //^^^ but bottom edge
		_camera.position.y = _camera.bounds.y + _camera.bounds.h - _camera.size.y;
	}

	if (_camera.position.y < _camera.bounds.y) { //^^^ but top edge
		_camera.position.y = _camera.bounds.y;
	}
}

void camera_center_on(GFC_Vector2D target) {
	GFC_Vector2D position;

	position.x = target.x - (_camera.size.x * 0.5);
	position.y = target.y - (_camera.size.y * 0.5);
	
	camera_set_position(position);

	if (_camera.bind_camera) {
		camera_apply_bounds();
	}
}
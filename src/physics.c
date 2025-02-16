#include "simple_logger.h"

#include "physics.h"

Physics_Object* physics_obj_new() {
	Physics_Object* obj;
    obj = malloc(sizeof(Physics_Object));
    if (!obj)
    {
        slog("failed to allocate space for physics obj");
        return NULL;
    }
	memset(obj, 0, sizeof(Physics_Object));
	return obj;
}


void physics_obj_free(Physics_Object* self) {
	if (!self) {
		return;
	}
	free(self);
	self = NULL;
}

void physics_obj_configure(Physics_Object* self) { // will eventually be configure from file
	if (!self) {
		return;
	}
	self->horizontal_velocity_cap = 2;
	self->downward_velocity_cap = 5;
}

void physics_update(Physics_Object* self) {
	GFC_Vector2D temp = gfc_vector2d(0,0);
	gfc_vector2d_add(temp, self->acceleration, FGRAV);
	gfc_vector2d_add(self->velocity, self->velocity, temp);
	if (self->velocity.x > self->horizontal_velocity_cap * (1 + self->running)) {
		self->velocity.x = self->horizontal_velocity_cap * (1 + self->running);
	}
	if (self->velocity.x < -self->horizontal_velocity_cap * (1 + self->running)) {
		self->velocity.x = -self->horizontal_velocity_cap * (1 + self->running);
	}
	if (self->velocity.y > self->downward_velocity_cap) {
		self->velocity.y = self->downward_velocity_cap;
	}
	gfc_vector2d_add(self->position, self->position, self->velocity);

	// v = v_0 + (a + g) * t
	// x = v * t
	
}


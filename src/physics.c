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

void physics_obj_configure(Physics_Object* self, SJson* json) {
	GFC_Vector4D bounds = { 0 };
	if ((!self) || (!json)) {
		slog("physics config fail");
		return;
	}

	sj_object_get_vector4d(json, "bounds", &bounds);
	self->bounds = gfc_rect_from_vector4(bounds);

	self->center = gfc_vector2d(bounds.x + (bounds.z / 2), bounds.y + (bounds.w / 2)); //the center of a rectangle is the point (x+w/2,y+h/2)

	//add to json file eventually?
	self->horizontal_velocity_cap = 2;
	self->downward_velocity_cap = 8;
}

void physics_update(Physics_Object* self) {

	/**		RELAVENT PHYSICS EQUATIONS
	*		v_now = v_last + a + g
	*		x_now = x_last + v_now
	*/

	if (!self) {
		return;
	}

	gfc_vector2d_add(self->velocity, self->velocity, self->acceleration); //add player movement acceleration to velocity
	gfc_vector2d_add(self->velocity, self->velocity, FGRAV); //apply gravity

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


	
}

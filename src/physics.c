#include "simple_logger.h"

#include "world.h"

#include "physics.h"

Physics_Object* physics_obj_new() {
	Physics_Object* obj;
    obj = gfc_allocate_array(sizeof(Physics_Object),1);
    if (!obj)
    {
        slog("failed to allocate space for physics obj");
        return NULL;
    }
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

	self->grounded = 0;
	//add to json file eventually?
	self->horizontal_velocity_cap = 2;
	self->downward_velocity_cap = 8; //should be the same (ish) as initial jump velocity
	self->horizontal_deceleration = 0.08;
	self->gravity = FGRAV;
}

GFC_Rect physics_obj_get_world_bounds_position(Physics_Object* self) {
	GFC_Rect bounds_pos = { 0 };
	if (!self) {
		slog("cannot get position of a physics object that doesnt exist");
		return bounds_pos;
	}
	gfc_rect_copy(bounds_pos, self->bounds);
	gfc_vector2d_add(bounds_pos, bounds_pos, self->position);
	gfc_vector2d_sub(bounds_pos, bounds_pos, self->center);
	return bounds_pos;
}

void physics_update(Physics_Object* self) { //check if collision happens after addition but before changing the variables, if there is, handle that and cancel movement?

	/**		RELAVENT PHYSICS EQUATIONS
	*		v_now = v_last + a + g
	*		x_now = x_last + v_now
	*/

	GFC_Vector2D test_velocity, test_position;
	GFC_Rect bounds;
	float horizontal_cap;
	float downward_cap;

	if (!self) {
		return;
	}


	gfc_vector2d_add(test_velocity, self->velocity, self->acceleration); //add player movement acceleration to velocity
	gfc_vector2d_add(test_velocity, test_velocity, self->gravity); //apply gravity

	horizontal_cap = self->override_horizontal_velocity_cap ? self->override_horizontal_velocity_cap : self->horizontal_velocity_cap * (1 + self->running);
	if (test_velocity.x > horizontal_cap) {
		test_velocity.x -= self->horizontal_deceleration;
	}
	if (test_velocity.x < -horizontal_cap) {
		test_velocity.x += self->horizontal_deceleration;
	}

	downward_cap = self->override_downward_velocity_cap ? self->override_downward_velocity_cap : self->downward_velocity_cap;
	if (test_velocity.y > downward_cap) {
		test_velocity.y = downward_cap;
	}

	gfc_vector2d_add(test_position, self->position, test_velocity);

	bounds = self->bounds;
	
	//test move in y direction first, then check if collision
	gfc_rect_copy(bounds, self->bounds);
	bounds.y = bounds.y + test_position.y;
	bounds.x = bounds.x + self->position.x;
	gfc_vector2d_sub(bounds, bounds, self->center);

	if (!world_test_collision_rect(world_get_active(), bounds)) { //no y direction collision
		//apply physics in the y direction:
		self->velocity.y = test_velocity.y;
		self->position.y = test_position.y;
		self->grounded = 0;
	}
	else { // y direction collision
		self->velocity.y = 0; //whether we bonked (velocity.y < 0) or are grounded (velocity.y > 0), set velocity.y to 0

		if (test_velocity.y > 0) { //if velocity is positive and a collision happened, we are grounded
			self->grounded = 1;
		}
		else {
			self->grounded = 0;
		}
	}

	bounds.y = self->bounds.y + self->position.y; //if no collision, self->position.y was changed, if not, not changed. either way, we need it for next bounds test
	bounds.x = self->bounds.x + test_position.x;
	gfc_vector2d_sub(bounds, bounds, self->center);


	if (!world_test_collision_rect(world_get_active(), bounds)) { //no x direction collision
		self->velocity.x = test_velocity.x;
		self->position.x = test_position.x;
		self->x_world_collision = 0;

	}
	else { //x direction collision
		self->velocity.x = 0;
		self->acceleration.x = 0;

		if (test_velocity.x > 0) {
			self->x_world_collision = 1;
		}
		else if (test_velocity.x < 0) {
			self->x_world_collision = -1;
		}
	}
}

Uint8 physics_obj_collision_check(Physics_Object* self, Physics_Object* other) { 
	GFC_Rect bounds1, bounds2;
	if ((!self) || (!other)) {
		return 0;
	}
	//if a team is none, clip against all
	//if teams are same, dont clip`
	//if no layer overlap, return 0

	bounds1 = physics_obj_get_world_bounds_position(self);
	bounds2 = physics_obj_get_world_bounds_position(other);

	//will eventually need to know what side the colission is for killing/dying, but not yet
	return gfc_rect_overlap(bounds1, bounds2);
}

#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "player.h"

#include "platform.h"

void platform_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* platform_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new platform entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new platform entity");
		return NULL;
	}
	entity_configure_from_def(self, "platform");

	entity_set_collision_layer(self, ECL_ALL);

	self->touch = platform_touch;

	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

void platform_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
	if (!self || !other) {
		return;
	}
	if (physics_obj_collision_check(self->physics, other->physics)) {
		return;
	}
	if (collision_side.y >= 0) {
		return;
	}
	other->physics->velocity.y = 0; //we are grounded (velocity.y > 0), set velocity.y to 0
	other->physics->grounded = 1;
	other->physics->y_col_this_frame = 1; //let physics_update to not try movement in the y direction
}



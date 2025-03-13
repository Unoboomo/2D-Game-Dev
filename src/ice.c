#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "player.h"

#include "ice.h"

void ice_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* ice_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new ice entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new ice entity");
		return NULL;
	}
	entity_configure_from_file(self, "def/ice.def");

	entity_set_collision_layer(self, ECL_World);

	self->touch = ice_touch;

	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

void ice_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
	if (!self || !other || !other->physics) {
		return;
	}
	if (!other->player) {
		return;
	}
	if (collision_side.y < 0) {
		other->physics->on_ice = 1;
		slog("icy");
	}
}



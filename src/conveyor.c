#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "player.h"

#include "conveyor.h"

void conveyor_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* conveyor_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new conveyor entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new conveyor entity");
		return NULL;
	}
	entity_configure_from_def(self, "conveyor");

	entity_set_collision_layer(self, ECL_ALL);
	self->team = ETT_world;

	self->touch = conveyor_touch;

	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

void conveyor_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
	if (!self || !other || !other->physics) {
		return;
	}

	if (collision_side.y >= 0) {
		return;
	}
	other->physics->next_frame_world_pos.x = 1;
}



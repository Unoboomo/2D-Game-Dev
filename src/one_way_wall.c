#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "player.h"

#include "one_way_wall.h"

void one_way_wall_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* one_way_wall_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new one_way_wall entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new one_way_wall entity");
		return NULL;
	}
	entity_configure_from_def(self, "one_way_wall");

	entity_set_collision_layer(self, ECL_ALL);

	self->touch = one_way_wall_touch;

	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

void one_way_wall_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
	if (!self || !other) {
		return;
	}
	if (physics_obj_collision_check(self->physics, other->physics)) {
		return;
	}
	if (collision_side.x <= 0) {
		return;
	}
	other->physics->velocity.x = 0; 
	other->physics->x_col_this_frame = 1;
	other->physics->x_world_collision = -1;

}



#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "bullet_bill.h"

void bullet_bill_think(Entity* self);
void bullet_bill_update(Entity* self);
void bullet_bill_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* bullet_bill_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new bullet_bill entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new bullet_bill entity");
		return NULL;
	}
	entity_configure_from_file(self, "def/bullet.def");

	entity_set_collision_layer(self, ECL_Entity);

	gfc_vector2d_copy(self->physics->position, position);
	self->physics->horizontal_velocity_cap = 4;
	self->physics->gravity = ZEROGRAV;
	self->think = bullet_bill_think;
	self->update = bullet_bill_update;
	self->touch = bullet_bill_touch;
	return self;
}

void bullet_bill_think(Entity* self) {
	float idle_dir = 0;
	if (!self || !self->physics) {
		return;
	}
	//entity idle movement, make function later?
	self->physics->velocity.x = self->physics->horizontal_velocity_cap;
}

void bullet_bill_update(Entity* self) {
	if (!self) {
		return;
	}
	entity_update_position(self);
}

void bullet_bill_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
	if (!self || !other) {
		return;
	}

	if (other->player) {
		if (collision_side.y < 0 && !physics_obj_collision_check(self->physics, other->physics)) {//if collision was top
			entity_free(self);
		}
		else {
			//damage player
		}
	}
}

#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "bug.h"

void bug_think(Entity* self);
void bug2_think(Entity* self);
void bug_update(Entity* self);
void bug_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* bug_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new bug entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new bug entity");
		return NULL;
	}
	entity_configure_from_file(self, "def/bug.def");

	entity_set_collision_layer(self, ECL_Entity);

	self->frame = 0;
	gfc_vector2d_copy(self->physics->position, position);

	self->think = bug_think;
	self->update = bug_update;
	self->touch = bug_touch;
	return self;
}

Entity* bug2_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new bug entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new bug entity");
		return NULL;
	}
	entity_configure_from_file(self, "def/bug.def");

	entity_set_collision_layer(self, ECL_Entity);
	self->physics->gravity = JUMPGRAV;
	self->frame = 0;
	gfc_vector2d_copy(self->physics->position, position);

	self->think = bug2_think;
	self->update = bug_update;
	self->touch = bug_touch;
	return self;
}
void bug_think(Entity* self) {
	float idle_dir = 0;
	if (!self || !self->physics) {
		return;
	}
	//entity idle movement, make function later?
	if (self->frame == 0) {
		idle_dir = (gfc_random() - 0.5) / 10;
		self->physics->acceleration.x = idle_dir;
	}
}

void bug2_think(Entity* self) {
	if (!self || !self->physics) {
		return;
	}
	if (self->physics->velocity.x == 0) {
		if (self->physics->x_world_collision) {
			self->physics->velocity.x = self->physics->horizontal_velocity_cap * -self->physics->x_world_collision;
		}
		else {
			if ((gfc_random() - 0.5) > 0) {
				self->physics->velocity.x = self->physics->horizontal_velocity_cap;
			}
			else {
				self->physics->velocity.x = -self->physics->horizontal_velocity_cap;

			}
		}
	}
	if (self->physics->grounded) {
		self->physics->velocity.y = -7;
	}
}

void bug_update(Entity* self) {
	if (!self) {
		return;
	}
	self->frame += 0.1;
	if (self->frame >= 16) {
		self->frame = 0;
	}
	entity_update_position(self);
}

void bug_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
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

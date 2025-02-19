#include "simple_logger.h"

#include "gfc_input.h"

#include "gf2d_graphics.h"

#include "bug.h"

void bug_think(Entity* self);
void bug_update(Entity* self);

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
	self->frame = 0;


	gfc_vector2d_copy(self->physics->position, position);

	self->think = bug_think;
	self->update = bug_update;
	
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

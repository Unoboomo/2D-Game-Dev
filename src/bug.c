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
	entity_configure_from_file(self, "def/bug.def");
	self->frame = 0;

	gfc_vector2d_copy(self->position, position);

	self->think = bug_think;
	self->update = bug_update;

	return self;
}

void bug_think(Entity* self) {
	int idle_dir;
	if (!self) {
		return;
	}
	//entity idle movement, make function later?
	if (self->frame == 0) {
		idle_dir = (int)(gfc_random() * 8);
		self->velocity = gfc_vector2d_from_angle((GFC_HALF_PI * idle_dir) / 2.0);
		gfc_vector2d_normalize(&self->velocity); //normalize velocity vector to fix faster diagonal movement
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

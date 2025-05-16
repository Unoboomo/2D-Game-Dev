#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "player.h"

#include "brick.h"

void brick_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* brick_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new brick entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new brick entity");
		return NULL;
	}
	entity_configure_from_def(self, "brick");

	entity_set_collision_layer(self, ECL_ALL);
	self->team = ETT_world;
	self->sound_effect = gfc_sound_load("audio/brick.wav", 1, 2);

	self->touch = brick_touch;
	
	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

void brick_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
	if (!self || !other || !other->physics) {
		return;
	}
	if (!other->player) {
		return;
	}
	if (collision_side.y <= 0) {
		return;
	}
	particles_from_def("brick_chunk_1", 4, self->physics->position, gfc_vector2d(0, -1), gfc_vector2d(0, 0.2));
	particles_from_def("brick_chunk_2", 3, self->physics->position, gfc_vector2d(0, -1), gfc_vector2d(0, 0.2));
	if (self->sound_effect) {
		gfc_sound_play(self->sound_effect, 0, 1, -1, -1);
		gfc_sound_free(self->sound_effect);
	}

	entity_free(self);
}



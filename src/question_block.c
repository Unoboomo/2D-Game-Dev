#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "player.h"

#include "question_block.h"

void question_block_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* question_block_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new question_block entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new question_block entity");
		return NULL;
	}
	entity_configure_from_def(self, "question_block");

	entity_set_collision_layer(self, ECL_ALL);
	self->team = ETT_world;
	self->sound_effect = gfc_sound_load("audio/brick.wav", 1, 2);

	self->touch = question_block_touch;

	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

Entity* invisible_question_block_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new question_block entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new question_block entity");
		return NULL;
	}
	entity_configure_from_def(self, "invisible_question_block");

	entity_set_collision_layer(self, ECL_ALL);
	self->team = ETT_world;

	self->touch = question_block_touch;

	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

Entity* empty_question_block_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new question_block entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new question_block entity");
		return NULL;
	}
	entity_configure_from_def(self, "empty_question_block");

	entity_set_collision_layer(self, ECL_ALL);
	self->team = ETT_world;

	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

void question_block_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
	GFC_Vector2D position;
	if (!self || !other) {
		return;
	}
	if (!other->player) {
		return;
	}
	if (physics_obj_collision_check(self->physics, other->physics)) {
		return;
	}
	if (collision_side.y <= 0) {
		return;
	}
	position = self->physics->position;
	if (self->sound_effect) {
		gfc_sound_play(self->sound_effect, 0, 1, -1, -1);
		gfc_sound_free(self->sound_effect);
	}
	entity_free(self);
	self = empty_question_block_new(position);
	//quick getaround for clipping
	other->physics->velocity.y = 0; //whether we bonked (velocity.y < 0) or are grounded (velocity.y > 0), set velocity.y to 0

	other->physics->y_col_this_frame = 1; //let physics_update to not try movement in the y direction
}




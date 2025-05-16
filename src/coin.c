#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "player.h"

#include "coin.h"

void coin_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* coin_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new coin entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new coin entity");
		return NULL;
	}
	entity_configure_from_def(self, "coin");

	entity_set_collision_layer(self, ECL_Item);

	self->touch = coin_touch;
	self->sound_effect = gfc_sound_load("audio/coin_recieved.wav", 1, 2);
	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

void coin_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
	PlayerEntityData* data;
	if (!self || !other) {
		return;
	}
	if (!other->player) {
		return;
	}
	data = (PlayerEntityData*)other->data;

	data->coin_count++;

	particles_from_def("sparkle", 10, self->physics->position, gfc_vector2d(0, -1), gfc_vector2d(0, 0.1));
	if (self->sound_effect) {
		gfc_sound_play(self->sound_effect, 0, 1, -1, -1);
		gfc_sound_free(self->sound_effect);
	}
	entity_free(self);
}



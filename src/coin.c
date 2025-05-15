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
	if (data->coin_count >= MAX_COINS) {
		data->coin_count -= MAX_COINS;
		if (data->lives_count < MAX_LIVES) {
			data->lives_count++;
		}
	}

	entity_free(self);
}



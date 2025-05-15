#include "simple_logger.h"

#include "gf2d_graphics.h"

#include "player.h"
#include "coin.h"

#include "p_switch.h"

void p_switch_touch(Entity* self, Entity* other, GFC_Vector2D collision_side);

Entity* p_switch_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new p_switch entity");
		return NULL;
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new p_switch entity");
		return NULL;
	}
	entity_configure_from_def(self, "p_switch");

	entity_set_collision_layer(self, ECL_Item);

	self->touch = p_switch_touch;

	self->frame = 0;
	self->physics->gravity = ZEROGRAV;
	gfc_vector2d_copy(self->physics->position, position);

	return self;
}

void p_switch_touch(Entity* self, Entity* other, GFC_Vector2D collision_side) {
	GFC_List* bricks;
	Entity* brick;
	GFC_Vector2D position;
	int c,i;
	if (!self || !other || !other->physics) {
		return;
	}
	if (physics_obj_collision_check(self->physics, other->physics)) {
		return;
	}
	if (collision_side.y >= 0) {
		return;
	}

	bricks = entity_find_by_name("brick");
	if (!bricks) {
		entity_free(self);
		return;
	}
	c = gfc_list_get_count(bricks);
	slog("%d", c);
	for (i = 0; i < c; i++) {
		brick = gfc_list_get_nth(bricks, i);
		if (!brick) {
			continue;
		}
		if (!brick->physics) {
			continue;
		}
		slog("here");

		position = brick->physics->position;
		entity_free(brick);
		brick = coin_new(position);
	}
	gfc_list_delete(bricks);

	entity_free(self);
}



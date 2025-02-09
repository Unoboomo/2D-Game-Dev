#include "simple_logger.h"

#include "gfc_input.h"

#include "gf2d_graphics.h"

#include "player.h"

void player_think(Entity* self);
void player_update(Entity* self);

Entity* player_new(GFC_Vector2D position) {
	Entity* self;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new player entity");
		return NULL; 
	}
	entity_configure_from_file(self, "def/player.def");
	self->frame = 0;

	gfc_vector2d_copy(self->position, position);

	self->think = player_think;
	self->update = player_update;

	return self;
}

void player_think(Entity* self) {
	if (!self) {
		return;
	}
	if (gfc_input_command_down("right")) {
		self->velocity.x = 1.0;
	}
	else if (gfc_input_command_down("left")) {
		self->velocity.x = -1.0;
	}
	else {
		self->velocity.x = 0;
	}
	if (gfc_input_command_down("up")) {
		self->velocity.y = -1.0;
	}
	else if (gfc_input_command_down("down")) {
		self->velocity.y = 1.0;
	}
	else {
		self->velocity.y = 0;
	}
	gfc_vector2d_normalize(&self->velocity); //normalize velocity vector to fix faster diagonal movement

	if (gfc_input_command_down("dash")) {
		gfc_vector2d_scale(self->velocity, self->velocity, 10);
	}
}

void player_update(Entity* self) {
	if (!self) {
		return;
	}
	self->frame += 0.1;
	if (self->frame >= 16) {
		self->frame = 0;
	}
	entity_update_position(self);
}

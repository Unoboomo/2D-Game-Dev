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

	self->physics = physics_obj_new();
	physics_obj_configure(self->physics);
	gfc_vector2d_copy(self->physics->position, position);
	
	self->think = player_think;
	self->update = player_update;
	

	return self;
}

void player_think(Entity* self) {
	if (!self) {
		return;
	}
	if (gfc_input_command_down("dash")) {
		self->physics->running = 1;
	}
	else {
		self->physics->running = 0;
	}

	if (gfc_input_command_down("right")) {
		self->physics->acceleration.x = 0.04 * (1 + self->physics->running);
		if (self->physics->velocity.x < 0) { //skid stops (quick change in direction from one way to the other
			self->physics->acceleration.x += 0.08;
		}
	}
	else if (gfc_input_command_down("left")) {
		self->physics->acceleration.x = -0.04 * (1 + self->physics->running);
		if (self->physics->velocity.x > 0) { //skid stops (quick change in direction from one way to the other
			self->physics->acceleration.x += -0.08;
		}
	}
	else {
		self->physics->acceleration.x = self->physics->velocity.x * -0.04;//-0.04 is the coefficient of friction of the surface the player is on
	}
	/*
	if (gfc_input_command_down("up")) {
		self->velocity.y = -1.0;
	}
	else if (gfc_input_command_down("down")) {
		self->velocity.y = 1.0;
	}
	else {
		self->velocity.y = 0;
	}
	*/
	if (gfc_input_command_pressed("jump")) {
		self->physics->velocity.y = -7;
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

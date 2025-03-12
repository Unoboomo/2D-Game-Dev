#include "simple_logger.h"

#include "gfc_input.h"

#include "gf2d_graphics.h"

#include "camera.h"

#include "player.h"

void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);


Entity* player_new(GFC_Vector2D position) {
	Entity* self;
	PlayerEntityData* data;
	self = entity_new();
	if (!self) {
		slog("failed to spawn a new player entity");
		return NULL; 
	}
	self->physics = physics_obj_new();
	if (!self->physics) {
		slog("failed to create a physics object for new player entity");
		return NULL;
	}
	entity_configure_from_file(self, "def/player.def");
	self->frame = 0;

	gfc_vector2d_copy(self->physics->position, position);
	
	self->think = player_think;
	self->update = player_update;
	self->free = player_free;
	
	data = gfc_allocate_array(sizeof(PlayerEntityData), 1);
	memset(data, 0, sizeof(PlayerEntityData));
	if (data) {
		//set data here
		self->data = data;
		slog("data is set");
	}
	else {
		slog("failed to create player_entity_data");
	}
	
	return self;
}

void player_free(Entity* self) {
	PlayerEntityData *data;
	if (!self || !self->data) {
		return;
	}
	data = self->data;
	//other cleanup here
	free(data);
	slog("data is freedd");
	self->data = NULL;
}

void player_think(Entity* self) {
	PlayerEntityData* data;
	int move_direction;
	float acceleration;
	float friction;

	if (!self|| !self->data) {
		return;
	}
	data = (PlayerEntityData*)self->data;

	if (self->physics->x_world_collision) { //collision with a wall
		data->last_x_collision_dir = self->physics->x_world_collision; //save the last collision direction
		if (!self->physics->grounded) { //wall sliding
			if (self->physics->velocity.y > WALL_SLIDE_SPEED) { //if falling faster than wallslide velocity, reset velocity
				self->physics->velocity.y = WALL_SLIDE_SPEED;
			}
			data->wall_jump_buffer = WALL_JUMP_BUFFER; //buffers a wall jump
		}
	}

	if (gfc_input_command_held("jump") || self->physics->velocity.y >= 0) { 
		self->physics->gravity = JUMPGRAV;
	}
	else { //jump held or falling, apply lighter gravity
		self->physics->gravity = FGRAV;
	}

	if (gfc_input_command_down("dash")) { //running or not
		self->physics->running = 1;
	}
	else {
		self->physics->running = 0;
	}

	if (gfc_input_command_down("right") != gfc_input_command_down("left")) { // Moving in a single direction
		move_direction = gfc_input_command_down("right") ? 1 : -1; // 1 for right, -1 for left

		// Running affects acceleration only on the ground
		acceleration = self->physics->grounded
			? GROUND_ACCELERATION * (1 + self->physics->running)
			: MIDAIR_ACCELERATION;

		self->physics->acceleration.x = move_direction * acceleration;

		// Skid stop: Extra acceleration when reversing direction
		if (self->physics->velocity.x * move_direction < 0) { // If moving in the opposite direction
			self->physics->acceleration.x += move_direction * (self->physics->grounded ? GROUND_ACCELERATION * (1 + self->physics->running) : MIDAIR_ACCELERATION);
		}
	}
	else { // No movement input
		friction = self->physics->grounded ? GROUND_ACCELERATION : MIDAIR_ACCELERATION;
		self->physics->acceleration.x = -self->physics->velocity.x * friction;
	}

	if (gfc_input_command_down("up")) { //dev flight, remove eventually
		self->physics->velocity.y = -6;
	}

	if (self->physics->x_world_collision > 0) {
		slog("right world collision");
	}
	if (self->physics->x_world_collision < 0) {
		slog("left world collision");
	}

	if (data->jump_buffer > 0) { 
		data->jump_buffer--;
	}
	if (data->wall_jump_buffer > 0) { 
		data->wall_jump_buffer--;
	}

	if (gfc_input_command_pressed("jump")) {
		if (self->physics->grounded) { //normal jump
			self->physics->velocity.y = JUMP_VELOCITY;
		}
		else if (self->physics->x_world_collision || data->wall_jump_buffer > 0) { //wall jump
			self->physics->velocity.x = -data->last_x_collision_dir * WALL_JUMP_VELOCITY;
			self->physics->velocity.y = JUMP_VELOCITY;
			data->wall_jump_buffer = 0;
		}
		else { //buffer a jump
			data->jump_buffer = JUMP_BUFFER;
		}
	}
	else if (self->physics->grounded && data->jump_buffer > 0) { //buffered jump
			self->physics->velocity.y = JUMP_VELOCITY;
			data->jump_buffer = 0;
	}
	else if ((self->physics->x_world_collision || data->wall_jump_buffer > 0) && data->jump_buffer > 0) { //buffered wall jump
		self->physics->velocity.x = -data->last_x_collision_dir * WALL_JUMP_VELOCITY;
		self->physics->velocity.y = JUMP_VELOCITY;
		data->wall_jump_buffer = 0;
		data->jump_buffer = 0;
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

	if (self->physics) {
		camera_center_on(self->physics->position);
	}
	

}

#include "simple_logger.h"

#include "gfc_input.h"

#include "gf2d_graphics.h"

#include "camera.h"
#include "world.h"

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

	self->player = 1;
	self->think = player_think;
	self->update = player_update;
	self->free = player_free;
	
	entity_set_collision_layer(self, ECL_ALL);

	data = gfc_allocate_array(sizeof(PlayerEntityData), 1);
	if (data) {
		//set data here
		self->data = data;
		data->lives_count = 5;
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
	self->data = NULL;
}

void player_think(Entity* self) {
	PlayerEntityData* data;
	int move_direction;
	float acceleration;
	float friction;
	GFC_Vector2D jump_velocity = { 0 };
	float x_velocity;

	if (!self|| !self->data) {
		return;
	}
	data = (PlayerEntityData*)self->data;
	gfc_vector2d_set(jump_velocity, self->physics->velocity.x, JUMP_VELOCITY);

	if (self->physics->x_world_collision) { //collision with a wall
		data->last_x_collision_dir = self->physics->x_world_collision; //save the last collision direction
		if (!self->physics->grounded) { //wall sliding
			if (self->physics->velocity.y > WALL_SLIDE_SPEED) { //if falling faster than wallslide velocity, reset velocity
				self->physics->velocity.y = WALL_SLIDE_SPEED;
			}
			data->wall_jump_buffer = WALL_JUMP_BUFFER; //buffers a wall jump
		}
	}
	if (data->jump_buffer > 0) {
		data->jump_buffer--;
	}
	if (data->wall_jump_buffer > 0) {
		data->wall_jump_buffer--;
	}
	if (data->ground_pound_delay > 0) {
		data->ground_pound_delay--;
	}
	if (data->ground_pound_recovery > 0) {
		data->ground_pound_recovery--;
	}

	if (gfc_input_command_pressed("down") && !self->physics->grounded && (!data->ground_pounding || data->ground_pound_delay == 1)) { //ground pounding logic (intentional infinite height glitch, frame perfect tho)
		data->ground_pounding = 1;
		data->ground_pound_delay = GROUND_POUND_DELAY;
		gfc_vector2d_set(self->physics->velocity, 0, GROUND_BOUND_BOOST);
		gfc_vector2d_clear(self->physics->acceleration);
		self->physics->gravity = ZEROGRAV;
		return;
	}
	if (data->ground_pounding) {
		if (data->ground_pound_delay > 0) {
			gfc_vector2d_clear(self->physics->velocity);
			return;
		}
		else {
			self->physics->gravity = FGRAV;
			self->physics->override_downward_velocity_cap = GROUND_POUND_VELOCITY_OVERRIDE;
		}
		if (self->physics->grounded || gfc_input_command_pressed("up")) {
			if (self->physics->grounded) {
				data->ground_pound_recovery = GROUND_POUND_RECOVERY;
			}
			data->ground_pounding = 0;
			self->physics->override_downward_velocity_cap = 0;
		}
	}

	if (!data->ground_pounding) { //which gravity to use when not groundpounding
		if (gfc_input_command_down("jump") || self->physics->velocity.y >= 0) {
			self->physics->gravity = JUMPGRAV;
		}
		else { //jump held or falling, apply lighter gravity
			self->physics->gravity = FGRAV;
		}
	}


	if (gfc_input_command_down("dash")) { //running or not
		self->physics->running = 1;
	}
	else {
		self->physics->running = 0;
	}


	if (gfc_input_command_down("down") && self->physics->grounded && !self->physics->override_horizontal_velocity_cap && !data->ground_pound_recovery) {// player starts crouching
		self->physics->bounds.h /= 2;
		self->physics->bounds.y += self->physics->bounds.h;
		self->physics->override_horizontal_velocity_cap = CROUCH_MAX_VELOCITY;
		self->physics->running = 0;
		data->crouching = 1;
	}
	if (data->crouching) {
		self->physics->running = 0;
		x_velocity = fabs(self->physics->velocity.x) > LONG_JUMP_THRESHOLD ? copysign(LONG_JUMP_VELOCITY, self->physics->velocity.x) : self->physics->velocity.x; //check to see if were moving fast enough to long jump
		gfc_vector2d_set(jump_velocity, x_velocity, CROUCH_JUMP_VELOCITY);
		if (!gfc_input_command_down("down")) {

			//resize hitbox to normal
			self->physics->bounds.y -= self->physics->bounds.h;
			self->physics->bounds.h *= 2;
			//test to see if can uncrouch
			if (!world_test_collision_rect(world_get_active(), physics_obj_get_world_bounds_position(self->physics))) { //ERROR HERE, doesnt check for entity world objects, will clip
				self->physics->override_horizontal_velocity_cap = 0;
				data->crouching = 0;
			}
			else {
				self->physics->bounds.h /= 2;
				self->physics->bounds.y += self->physics->bounds.h;
			}
		}
	}
	if (self->physics->grounded) {
		acceleration = friction = GROUND_ACCELERATION * (self->physics->on_ice ? ICE_ACCELERATION_MODIFIER : 1);
		// Running affects acceleration only on the ground
		acceleration *= (1 + self->physics->running);
	}
	else {
		acceleration = friction = MIDAIR_ACCELERATION;
	}

	if (gfc_input_command_down("right") != gfc_input_command_down("left")) { // Moving in a single direction
		move_direction = gfc_input_command_down("right") ? 1 : -1; // 1 for right, -1 for left


		self->physics->acceleration.x = move_direction * acceleration;

		// Skid stop: Extra acceleration when reversing direction
		if (self->physics->velocity.x * move_direction < 0) { // If moving in the opposite direction
			self->physics->acceleration.x += move_direction * acceleration;
		}
	}
	else { // No movement input
		self->physics->acceleration.x = -self->physics->velocity.x * friction;
	}

	if (gfc_input_command_pressed("jump")) {
		if (self->physics->grounded) { //normal jump
			self->physics->velocity = jump_velocity;
		}
		else if (self->physics->x_world_collision || data->wall_jump_buffer > 0) { //wall jump
			self->physics->velocity = gfc_vector2d(-data->last_x_collision_dir * WALL_JUMP_VELOCITY, JUMP_VELOCITY);
			data->wall_jump_buffer = 0;
		}
		else { //buffer a jump
			data->jump_buffer = JUMP_BUFFER;
		}
	}
	else if (self->physics->grounded && data->jump_buffer > 0) { //buffered jump
			self->physics->velocity = jump_velocity;
			data->jump_buffer = 0;
	}
	else if ((self->physics->x_world_collision || data->wall_jump_buffer > 0) && data->jump_buffer > 0) { //buffered wall jump
		self->physics->velocity = gfc_vector2d(-data->last_x_collision_dir * WALL_JUMP_VELOCITY, JUMP_VELOCITY);
		data->wall_jump_buffer = 0;
		data->jump_buffer = 0;
	}

	if (data->ground_pound_recovery) {
		if (self->physics->velocity.y < 0) { //we jumped out of the recovery
			data->ground_pound_recovery = 0;
			self->physics->velocity.y *= GROUND_POUND_RECOVERY_JUMP_BOOST; //groundpound jump boost
		}
		else if (self->physics->grounded) {
			self->physics->acceleration.x = self->physics->velocity.x = 0;
		}
	}

	self->physics->on_ice = 0;
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

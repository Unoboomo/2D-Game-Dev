#include "simple_logger.h"

#include "gfc_config.h"
#include "gfc_config_def.h"
#include "gfc_list.h"

#include "gf2d_draw.h"
#include "gf2d_graphics.h"

#include "camera.h"

#include "entity.h"

typedef struct
{
	Uint32 entity_max;
	Entity* entity_list;
}EntitySystem;

extern Uint8 _DRAWBOUNDS; //get external variable, this one is in game.c

static EntitySystem entity_system = { 0 };

/**
* @brief draws an entity to the screen
* @param *self a pointer to the entity to draw
*/
void entity_draw(Entity* self); 

void entity_system_close()
{
	entity_system_free_all();
	if (entity_system.entity_list) {
		free(entity_system.entity_list);
		entity_system.entity_list = NULL;
	}
	memset(&entity_system, 0, sizeof(EntitySystem));
	slog("entity system closed");
}

void entity_system_init(Uint32 max)
{
	if (entity_system.entity_list) {
		slog("cannot initialize multiple entity systems");
		return;
	}
	if (!max)
	{
		slog("cannot initialize entity system for Zero entities");
		return;
	}
	entity_system.entity_max = max;
	entity_system.entity_list = gfc_allocate_array(sizeof(Entity), max);
	if (!entity_system.entity_list) {
		slog("failed to allocate global entity list");
		return;
	}
	slog("entity system initialized");
	atexit(entity_system_close);
}

void entity_system_free_all() {
	int i;
	for (i = 0; i < entity_system.entity_max; i++) {
		if (entity_system.entity_list[i]._inuse) {
			entity_free(&entity_system.entity_list[i]);
		}
	}
}


void entity_system_draw_all() {
	int i;
	for (i = 0; i < entity_system.entity_max; i++) {
		if (entity_system.entity_list[i]._inuse) {
			entity_draw(&entity_system.entity_list[i]);
		}
	}
}

void entity_system_think_all() {
	int i;
	for (i = 0; i < entity_system.entity_max; i++) {
		if (!entity_system.entity_list[i]._inuse) {
			continue;
		}
		if (!entity_system.entity_list[i].think) {
			continue;
		}
		entity_system.entity_list[i].think(&entity_system.entity_list[i]);
	}
}

void entity_system_update_all() {
	int i;
	for (i = 0; i < entity_system.entity_max; i++) {
		if (!entity_system.entity_list[i]._inuse) {
			continue;
		}
		if (!entity_system.entity_list[i].update) {
			continue;
		}
		entity_system.entity_list[i].update(&entity_system.entity_list[i]);
	}
}

Entity* entity_new() {
	int i;
	for (i = 0; i < entity_system.entity_max; i++) {
		if (entity_system.entity_list[i]._inuse) {
			continue;
		}
		memset(&entity_system.entity_list[i], 0, sizeof(Entity));
		entity_system.entity_list[i]._inuse = 1;
		return &entity_system.entity_list[i];
	}
	slog("failed to allocate new entity: list full");
	return NULL;
}

void entity_free(Entity* self) {
	if (!self) {
		return;
	}
	if (self->sprite) {
		gf2d_sprite_free(self->sprite);
	}
	if (self->physics) {
		physics_obj_free(self->physics);
	}
	if (self->free) {
		self->free(self);
	}
	//anything else for our entity would be freed here
	memset(self, 0, sizeof(Entity));
}



void entity_draw(Entity* self) {
	GFC_Rect rect;
	GFC_Rect center_pos;
	GFC_Vector2D draw_pos = { 0 };
	GFC_Vector2D camera_offset;
	if ((!self) || (!self->sprite) || (!self->physics)) {
		return;
	}
	camera_offset = camera_get_offset();

	gfc_vector2d_sub(draw_pos, self->physics->position, self->physics->center); //draw position is different that the position because the entity is centered on the position
	gfc_vector2d_add(draw_pos, draw_pos, camera_offset);
	gf2d_sprite_draw(
		self->sprite,
		draw_pos,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(Uint32)self->frame);

	if (_DRAWBOUNDS) {
		rect = physics_obj_get_world_bounds_position(self->physics);
		gfc_vector2d_add(rect, rect, camera_offset);

		gf2d_draw_rect(rect, GFC_COLOR_YELLOW);

		center_pos = gfc_rect(self->physics->position.x - 3, self->physics->position.y - 3, 5, 5); 
		gfc_vector2d_add(center_pos, center_pos, camera_offset);
		gf2d_draw_rect_filled(center_pos, GFC_COLOR_YELLOW);
	}
}

void entity_configure_from_def(Entity* self, const char* name) {
	SJson* json;

	if (!self || !name) {
		slog("cannot configure entity that does not exist or has no name");
	}
	json = gfc_config_def_get_by_name("entities", name); //may need to change the first parameter based on where entities end up, add new parameter to the function?

	if (!json) {
		slog("json for %s does not exist",name);
		return;
	}

	entity_configure_from_json(self, json);
}

GFC_Rect entity_bounds_from_def(const char* name) {
	SJson* json;
	GFC_Vector4D bounds_vec = { 0 };
	GFC_Rect bounds = { 0 };

	if (!name) {
		slog("cannot configure entity that does not exist or has no name");
		return bounds;
	}
	json = gfc_config_def_get_by_name("entities", name); //may need to change the first parameter based on where entities end up, add new parameter to the function?

	if (!json) {
		slog("json for %s does not exist", name);
		return bounds;
	}
	if (sj_object_get_vector4d(json, "bounds", &bounds_vec)) {
		slog("got it");
		bounds = gfc_rect_from_vector4(bounds_vec);
		return bounds;
	}
	return bounds;
}

void entity_configure_from_file(Entity* self, const char* filename) {
	SJson* json;
	if (!self) {
		return;
	}
	json = sj_load(filename);
	if (!json) {
		slog("json file does not exist");
		return;
	}
	entity_configure_from_json(self, json);
	sj_free(json);
}

void entity_configure_from_json(Entity* self, SJson* json)
{
	GFC_Vector2D frame_size = { 0 };
	Uint32 frames_per_line = 0;
	const char* sprite = NULL;
	if ((!self) || (!json)) {
		return;
	}

	sprite = sj_object_get_string(json, "sprite");
	if (sprite) {
		sj_object_get_vector2d(json, "sprite_size", &frame_size);
		sj_object_get_uint32(json, "sprite_fpl", &frames_per_line);
		self->sprite = gf2d_sprite_load_all(
			sprite,
			(Uint32)frame_size.x,
			(Uint32)frame_size.y,
			frames_per_line,
			0);
	}
	sprite = sj_object_get_string(json, "name");
	if (sprite) {
		gfc_line_cpy(self->name, sprite);
	}

	physics_obj_configure(self->physics, json);
}
/**
* I DONT KNOW FI THIS WORKS
SJson* entity_save_all_to_config() {
	SJson* json;
	SJson* entity;
	SJson* position;
	int i,j;

	json = sj_array_new();

	if (!json) {
		slog("failed to create an array for entities");
		return NULL;
	}

	entity = sj_object_new();

	if (!entity) {
		slog("failed to create new entity object");
		sj_free(json);
		return NULL;
	}

	position = sj_array_new();
	if (!position) {
		slog("failed to create new position object");
		sj_free(json);
		sj_free(entity);
		return NULL;
	}

	for (i = 0; i < entity_system.entity_max; i++) {
		if (entity_system.entity_list[i]._inuse) {
			sj_array_append(position, sj_new_float(entity_system.entity_list[i].physics->position.x));
			sj_array_append(position, sj_new_float(entity_system.entity_list[i].physics->position.y));
			sj_object_insert(entity, "name", sj_new_str(entity_system.entity_list[i].name));
			sj_object_insert(entity, "spawn_position", sj_copy(position));

			sj_array_append(json, sj_copy(entity));

			sj_object_delete_key(entity, "name");
			sj_object_delete_key(entity, "spawn_position");

			for (j = sj_array_count(position) - 1; j >= 0; j--) {
				sj_array_delete_nth(position, j);
			}
		}
	}
	sj_free(entity);
	sj_free(position);
	return json;
}
*/

void entity_update_position(Entity* self) {

	if (!self || !self->physics) {
		return; 
	}
	
	entity_collide_all(self);
	
	physics_update(self->physics);

	entity_check_world_bounds(self);
}

Uint8 entity_check_team(Entity* self, EntityTeamType team) {
	if (!self) {
		return ETT_none;
	}
	return self->team & team;
}

Uint8 entity_check_layer(Entity* self, EntityCollisionLayers layer) {
	if (!self) {
		return ECL_none;
	}
	return self->layer & layer;
}

void entity_set_collision_layer(Entity* self, EntityCollisionLayers layer) {
	if (!self) {
		return;
	}
	self->layer |= layer;
}

void entity_remove_collision_layer(Entity* self, EntityCollisionLayers layer) {
	if (!self) {
		return;
	}
	self->layer &= ~layer;
}


void entity_check_world_bounds(Entity* self) {
	GFC_Rect bounds;

	if (!self || !self->physics) {
		return;
	}

	bounds = camera_get_bounds();

	//checks for colisions with the edge of the world and prevents leaving the world (world bounds = camera bounds)
	if (self->physics->position.x - self->physics->bounds.w / 2 < bounds.x) {
		self->physics->position.x = bounds.x + self->physics->bounds.w / 2;
		self->physics->x_world_collision = -1;
		self->physics->velocity.x = 0;
	}
	if (self->physics->position.x + self->physics->bounds.w / 2 > bounds.x + bounds.w) {
		self->physics->position.x = bounds.x + bounds.w - self->physics->bounds.w / 2;
		self->physics->x_world_collision = 1;
		self->physics->velocity.x = 0;
	}
	if (self->physics->position.y - self->physics->bounds.h / 2 < bounds.y) {
		self->physics->position.y = bounds.y + self->physics->bounds.h / 2;
	}
	if (self->physics->position.y + self->physics->bounds.h / 2 > bounds.y + bounds.h) {
		self->physics->position.y = bounds.y + bounds.h - self->physics->bounds.h / 2;
	}
}


void entity_collide_all(Entity* self) {
	int i;
	GFC_Rect bounds;
	GFC_Vector2D test_position;
	GFC_Vector2D velocity_dir;
	GFC_Vector2D collision_side; //this is in relation to the entity we collided with, if velocity.y is + (self moving down), this is (0,-1), etc.

	if (!self || !self->physics) {
		return;
	}

	self->physics->x_col_this_frame = self->physics->y_col_this_frame = 0; //reset frame collisions

	test_position = physics_get_test_position(self->physics);

	//finds the direction the entity wants to move to determine collision sides
	velocity_dir = gfc_vector2d(test_position.x - self->physics->position.x, test_position.y - self->physics->position.y);
	
	//get collision bounds for y direction test
	gfc_rect_copy(bounds, self->physics->bounds);
	bounds.y = bounds.y + test_position.y;
	bounds.x = bounds.x + self->physics->position.x;
	gfc_vector2d_sub(bounds, bounds, self->physics->center);

	for (i = 0; i < entity_system.entity_max; i++) { //y change test
		if (!entity_system.entity_list[i]._inuse|| self == &entity_system.entity_list[i] || entity_system.entity_list[i].player) { //currently, things dont collide with the player, the player collides with things
			continue;
		}
		if (entity_check_team(self, entity_system.entity_list[i].team)) { //if on the same team, dont collide
			continue;
		}
		if (!entity_check_layer(self, entity_system.entity_list[i].layer)) { //if not on the same layer, dont collide
			continue;
		}
		if (physics_obj_test_collision_rect(entity_system.entity_list[i].physics, bounds)) {

			if (entity_check_team(&entity_system.entity_list[i], ETT_world)) { //if the other entity is of team world, we treat as a world collision

				self->physics->velocity.y = 0; //whether we bonked (velocity.y < 0) or are grounded (velocity.y > 0), set velocity.y to 0

				if (velocity_dir.y > 0) { //if velocity is positive and a collision happened, we are grounded
					self->physics->grounded = 1;
				}
				else { //otherwise, we bonked, but we dont care about bonking
					self->physics->grounded = 0;
				}

				self->physics->y_col_this_frame = 1; //there was a y collision this frame, so pass the info along to the physics_update function
			}

			if (velocity_dir.y > 0) { //if self is moving down
				collision_side = TOP_SIDE; //hit the top side of other entity
			}
			else { //if self is moving up
				collision_side = BOTTOM_SIDE; //hit the bottom side of other entity
			}

			/*
			if (!strcmp(self->name, "player_class")) { //if the entity is a player, what is the player colliding with
				slog("%s is colliding with %s", self->name, entity_system.entity_list[i].name);
			}
			*/

			if (entity_system.entity_list[i].touch) { //if the entity has a touch function, call it
				entity_system.entity_list[i].touch(&entity_system.entity_list[i], self, collision_side);
			}
		}
	}

	//get collision bounds for x direction test
	gfc_rect_copy(bounds, self->physics->bounds);
	bounds.y = self->physics->y_col_this_frame ? bounds.y + self->physics->position.y : bounds.y + test_position.y;
	bounds.x = bounds.x + test_position.x;
	gfc_vector2d_sub(bounds, bounds, self->physics->center);

	for (i = 0; i < entity_system.entity_max; i++) { //x change test
		if (!entity_system.entity_list[i]._inuse || self == &entity_system.entity_list[i] || entity_system.entity_list[i].player) { //currently, things dont collide with the player, the player collides with things
			continue;
		}
		if (entity_check_team(self, entity_system.entity_list[i].team)) { //if on the same team, dont collide
			continue;
		}
		if (!entity_check_layer(self, entity_system.entity_list[i].layer)) { //if not on the same layer, dont collide
			continue;
		}
		if (physics_obj_test_collision_rect(entity_system.entity_list[i].physics, bounds)) {

			if (entity_check_team(&entity_system.entity_list[i], ETT_world)) { //if the other entity is of team world, we treat as a world collision

				self->physics->velocity.x = 0;
				self->physics->acceleration.x = 0;

				if (velocity_dir.x > 0) {
					self->physics->x_world_collision = 1;
				}
				else if (velocity_dir.x < 0) {
					self->physics->x_world_collision = -1;
				}

				self->physics->x_col_this_frame = 1; //let physics_update to not try movement in the x direction

			}

			if (velocity_dir.x > 0) { //if self is moving right
				collision_side = LEFT_SIDE; //hit the left side of other entity
			}
			else { //if self is moving left
				collision_side = RIGHT_SIDE; //hit the right side of other entity
			}

			/*
			if (!strcmp(self->name, "player_class")) { //if the entity is a player, what is the player colliding with
				slog("%s is colliding with %s", self->name, entity_system.entity_list[i].name);
			}
			*/
			
			if (entity_system.entity_list[i].touch) { //if the entity has a touch function, call it
				entity_system.entity_list[i].touch(&entity_system.entity_list[i], self, collision_side);
			}
			
		}
	}
}

GFC_List* entity_find_by_name(const char* entity_name) {
	int i;
	GFC_List* entities = gfc_list_new();

	for (i = 0; i < entity_system.entity_max; i++) { //y change test
		if (!entity_system.entity_list[i]._inuse) {
			continue;
		}
		if (!strcmp(entity_name, entity_system.entity_list[i].name)) {
			gfc_list_append(entities, &entity_system.entity_list[i]);
		}
	}
	if (!gfc_list_get_count(entities)) {
		gfc_list_delete(entities);
		slog("did not find any entities of the name %s", entity_name);
		return NULL;
	}
	return entities;
}

Uint8 entity_test_collision_rect(GFC_Rect bounds) {
	int i;
	GFC_Rect other_bounds;
	for (i = 0; i < entity_system.entity_max; i++) { //y change test
		if (!entity_system.entity_list[i]._inuse) { 
			continue;
		}
		if (physics_obj_test_collision_rect(entity_system.entity_list[i].physics, bounds)) {
			other_bounds = physics_obj_get_world_bounds_position(entity_system.entity_list[i].physics);
			return 1;
		}
	}
	return 0;
}

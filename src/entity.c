#include "simple_logger.h"

#include "gfc_config.h"
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

		center_pos = gfc_rect(self->physics->position.x - 2, self->physics->position.y - 2, 5, 5); 
		gfc_vector2d_add(center_pos, center_pos, camera_offset);
		gf2d_draw_rect_filled(center_pos, GFC_COLOR_YELLOW);
	}
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
	entity_configure(self, json);
	sj_free(json);
}

void entity_configure(Entity* self, SJson* json)
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

void entity_update_position(Entity* self) {

	if (!self || !self->physics) {
		return; 
	}
	
	entity_collide_all(self);
	
	physics_update(self->physics);

	entity_check_world_bounds(self);
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
	}
	if (self->physics->position.x + self->physics->bounds.w / 2 > bounds.x + bounds.w) {
		self->physics->position.x = bounds.x + bounds.w - self->physics->bounds.w / 2;
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
	self->physics->x_collided_prev = self->physics->y_collided_prev = 0;
	//get test position of entity
	test_position = physics_get_test_position(self->physics);

	velocity_dir = gfc_vector2d(test_position.x - self->physics->position.x, test_position.y - self->physics->position.y);
	
	//get collision bounds for y direction test
	gfc_rect_copy(bounds, self->physics->bounds);
	bounds.y = bounds.y + test_position.y;
	bounds.x = bounds.x + self->physics->position.x;
	gfc_vector2d_sub(bounds, bounds, self->physics->center);

	for (i = 0; i < entity_system.entity_max; i++) { //y change test
		if (!entity_system.entity_list[i]._inuse|| self == &entity_system.entity_list[i] || entity_system.entity_list[i].player) {
			continue;
		}
		if (!entity_check_layer(self, entity_system.entity_list[i].layer)) {
			continue;
		}
		if (physics_obj_test_collision_rect(entity_system.entity_list[i].physics, bounds)) {

			if (entity_check_layer(&entity_system.entity_list[i], ECL_World)) { //if this, we treat as a world collision
				self->physics->velocity.y = 0; //whether we bonked (velocity.y < 0) or are grounded (velocity.y > 0), set velocity.y to 0

				if (velocity_dir.y > 0) { //if velocity is positive and a collision happened, we are grounded
					self->physics->grounded = 1;
				}
				else {
					self->physics->grounded = 0;
				}
				self->physics->y_collided_prev = 1; //let physics_update to not try movement in the y direction
			}
			collision_side = velocity_dir.y > 0 ? gfc_vector2d(0, -1) : gfc_vector2d(0, 1);
			if (!strcmp(self->name, "player_class")) {
				//slog("%s is colliding with %s", self->name, entity_system.entity_list[i].name);
			}
			
			if (entity_system.entity_list[i].touch) {
				entity_system.entity_list[i].touch(&entity_system.entity_list[i], self, collision_side);
			}
			if (self->physics->y_collided_prev) {
				test_position.y = self->physics->position.y;
			}
		}
	}

	//get collision bounds for x direction test
	bounds.y = self->physics->bounds.y + test_position.y; //logic here is wonky if we have a worldlike collision in the y direction
	bounds.x = self->physics->bounds.x + test_position.x;
	gfc_vector2d_sub(bounds, bounds, self->physics->center);

	for (i = 0; i < entity_system.entity_max; i++) { //x change test
		if (!entity_system.entity_list[i]._inuse || self == &entity_system.entity_list[i]) {
			continue;
		}
		if (!entity_check_layer(self, entity_system.entity_list[i].layer)) {
			continue;
		}
		if (physics_obj_test_collision_rect(entity_system.entity_list[i].physics, bounds)) {
			if (entity_check_layer(&entity_system.entity_list[i], ECL_World)) { //if this, we treat as a world collision
				self->physics->velocity.x = 0;
				self->physics->acceleration.x = 0;

				if (velocity_dir.x > 0) {
					self->physics->x_world_collision = 1;
				}
				else if (velocity_dir.x < 0) {
					self->physics->x_world_collision = -1;
				}

				self->physics->x_collided_prev = 1; //let physics_update to not try movement in the x direction

			}
			collision_side = velocity_dir.x > 0 ? gfc_vector2d(-1, 0) : gfc_vector2d(1,0);

			if (!strcmp(self->name, "player_class")) {
				//slog("%s is colliding with %s", self->name, entity_system.entity_list[i].name);
			}
			
			if (entity_system.entity_list[i].touch) {
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

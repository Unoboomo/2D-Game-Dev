#include "simple_logger.h"

#include "gfc_config.h"

#include "gf2d_graphics.h"

#include "entity.h"

typedef struct
{
	Uint32 entity_max;
	Entity* entity_list;
}EntitySystem;

static EntitySystem entity_system = { 0 };

/**
* @brief draws an entity to the screen
* @param *self a pointer to the entity to draw
*/
void entity_draw(Entity* self); 

void entity_system_close()
{
	entity_system_free_all();
	if (!entity_system.entity_list) {
		free(entity_system.entity_list);
	}
	memset(&entity_system, 0, sizeof(EntitySystem));
	slog("entity system closed");

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
	GFC_Vector4D bounds = { 0 };
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
	sj_object_get_string(json, "name");
	if (sprite) {
		gfc_line_cpy(self->name, sprite);
	}
	sj_object_get_vector4d(json, "bounds", &bounds);
	self->hitbox = gfc_rect_from_vector4(bounds);
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
	//anything else for our entity would be freed here
	memset(self, 0, sizeof(Entity));
}

void entity_draw(Entity* self) {
	if (!self) {
		return;
	}
	if (!self->sprite) {
		return;
	}
	gf2d_sprite_draw(
		self->sprite,
		self->position,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(Uint32)self->frame);
}

void entity_update_position(Entity* self) {
	GFC_Vector2D screen;
	if (!self) {
		return;
	}

	gfc_vector2d_add(self->position, self->position, self->velocity);

	screen = gf2d_graphics_get_resolution();
	//checks for colisions with the edge of the window and prevents leaving the window
	if (self->position.x + self->hitbox.x < 0) {
		self->position.x = 0 - self->hitbox.x;
	}
	if (self->position.x + self->hitbox.x + self->hitbox.w > screen.x) {
		self->position.x = screen.x - self->hitbox.x - self->hitbox.w;
	}
	if (self->position.y + self->hitbox.y < 0) {
		self->position.y = 0 - self->hitbox.y;
	}
	if (self->position.y + self->hitbox.y + self->hitbox.h > screen.y) {
		self->position.y = screen.y - self->hitbox.y - self->hitbox.h;
	}
}
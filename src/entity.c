#include "simple_logger.h"

#include "gfc_config.h"

#include "gf2d_draw.h"
#include "gf2d_graphics.h"

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
	//anything else for our entity would be freed here
	memset(self, 0, sizeof(Entity));
}



void entity_draw(Entity* self) {
	GFC_Rect rect;
	GFC_Rect center_pos;
	GFC_Vector2D draw_pos = { 0 };
	if ((!self) || (!self->sprite) || (!self->physics)) {
		return;
	}

	gfc_vector2d_sub(draw_pos, self->physics->position, self->physics->center); //draw position is different that the position because the entity is centered on the position

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
		gfc_rect_copy(rect, self->physics->bounds);
		gfc_vector2d_add(rect, rect, self->physics->position);
		gfc_vector2d_sub(rect, rect, self->physics->center);
		gf2d_draw_rect(rect, GFC_COLOR_YELLOW);

		center_pos = gfc_rect(self->physics->position.x - 2, self->physics->position.y - 2, 5, 5); 
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
	GFC_Vector2D screen;
	if (!self || !self->physics) {
		return;
	}

	physics_update(self->physics);

	screen = gf2d_graphics_get_resolution();

	//checks for colisions with the edge of the window and prevents leaving the window
	if (self->physics->position.x - self->physics->bounds.w / 2 < 0) {
		self->physics->position.x = self->physics->bounds.w / 2;
	}
	if (self->physics->position.x + self->physics->bounds.w / 2 > screen.x) {
		self->physics->position.x = screen.x - self->physics->bounds.w / 2;
	}
	if (self->physics->position.y - self->physics->bounds.h / 2 < 0) {
		self->physics->position.y = self->physics->bounds.h / 2;
	}
	if (self->physics->position.y + self->physics->bounds.h / 2 > screen.y) {
		self->physics->position.y = screen.y - self->physics->bounds.h / 2;
	}
}
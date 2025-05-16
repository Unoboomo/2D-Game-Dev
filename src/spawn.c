#include "simple_logger.h"

#include "gfc_config.h"

#include "world.h"

#include "player.h"

#include "brick.h"
#include "bug.h"
#include "bullet_bill.h"
#include "coin.h"
#include "conveyor.h"
#include "ice.h"
#include "one_way_wall.h"
#include "platform.h"
#include "p_switch.h"
#include "question_block.h"
#include "trampoline.h"

#include "spawn.h"

static SpawnLinks spawnlist[] =
{
	{
		"brick",
		brick_new
	},
	{
		"bug",
		bug_new
	},
	{
		"bug2",
		bug2_new
	},
	{
		"bullet_bill",
		bullet_bill_new
	},
	{
		"coin",
		coin_new
	},
	{
		"conveyor",
		conveyor_new
	},
	{
		"ice",
		ice_new
	},
	{
		"one_way_wall",
		one_way_wall_new
	},
	{
		"platform",
		platform_new
	},
	{
		"player",
		player_new
	},
	{
		"p_switch",
		p_switch_new
	},
	{
		"question_block",
		question_block_new
	},
	{
		"invisible_question_block",
		bug2_new
	},
	{
		"empty_question_block",
		empty_question_block_new
	},
	{
		"trampoline",
		trampoline_new
	}
};

size_t list_length = sizeof(spawnlist) / sizeof(spawnlist[0]);
/**
* I Dont Know if This Works
void spawn_entities_from_json(SJson* json) {
	int count;
	int i;
	SJson* entity;
	const char* name;
	int found = 0;
	GFC_Vector2D spawn_position = { 0 }; //represents the tilemap position, not the world position. 
	if (!json) {
		slog("cannot spawn entities from a json that does not exist");
		return;
	}

	if (!sj_is_array(json)) {
		slog("entity array is not an array");
		return;
	}

	count = sj_array_get_count(json);
	for (i = 0; i < count; i++) {
		found = 0;
		name = NULL;
		entity = NULL;
		gfc_vector2d_clear(spawn_position);

		entity = sj_array_get_nth(json, i);
		if (!entity) {
			slog("%dth entity data missing", i);
			continue;
		}

		name = sj_object_get_value_as_string(entity, "name");
		if (!name) {
			slog("%dth entity missing name",i);
			continue;
		}

		if (!sj_object_get_vector2d(entity, "spawn_position", &spawn_position)) {
			slog("%dth entity missing spawn position", i);
			continue;
		}
		
		
		for (i = 0; i < list_length; i++) {
			if (strcmp(spawnlist[i].name, name) == 0) {
				found = 1;
				break;
			}
		}

		if (!found) {
			slog("No spawn function found for entity name: %s", name);
			return;
		}

		spawnlist[i].spawn(spawn_position);
	}
}
*/

void spawn_entity_to_world(GFC_Vector2D spawn_position, const char* entity_name, const char* contents) {
	GFC_Rect bounds = { 0 };
	int i;
	int found = 0;

	bounds = entity_bounds_from_def(entity_name);
	bounds = get_world_bounds_position(spawn_position, bounds);
	if (spawn_check_bounds_against_world(bounds)) {
		slog("cannot spawn an entity here, it would collide with the world");
		return;
	}
	if (spawn_check_bounds_against_entities(bounds)) {
		slog("cannot spawn an entity here, it would collide with another entity");
		return;
	}

	for (i = 0; i < list_length; i++) {
		if (strcmp(spawnlist[i].name, entity_name) == 0) {
			found = 1;
			break;
		}
	}

	if (!found) {
		slog("No spawn function found for entity name: %s", entity_name);
		return;
	}

	spawnlist[i].spawn(spawn_position);
}

void spawn_entity_to_world_gridlock(GFC_Vector2D spawn_position, const char* entity_name, const char* contents) {
	World* world;
	GFC_Vector2D new_spawn_position = { 0 };
	world = world_get_active();

	if (!world) {
		slog("no active world to spawn an entity to");
		return;
	}
	if (!world->tileset) {
		slog("no way to find tile width and tile height");
		return;
	}
	new_spawn_position.x = (int) spawn_position.x - ((int) spawn_position.x % world->tileset->frame_w);
	new_spawn_position.y = (int) spawn_position.y - ((int) spawn_position.y % world->tileset->frame_h);

	gfc_vector2d_add(new_spawn_position, new_spawn_position, gfc_vector2d(world->tileset->frame_w / 2 + 1, world->tileset->frame_h / 2 + 1));

	spawn_entity_to_world(new_spawn_position, entity_name, contents);
}

GFC_Rect get_world_bounds_position(GFC_Vector2D spawn_position, GFC_Rect bounds) {
	GFC_Rect bounds_pos = { 0 };
	GFC_Vector2D center;

	center = gfc_vector2d(bounds.x + ((int)bounds.w / 2+ 1), bounds.y + ((int)bounds.h / 2 + 1)); //the center of a rectangle is the point (x+w/2,y+h/2)
	gfc_rect_copy(bounds_pos, bounds);
	gfc_vector2d_add(bounds_pos, bounds_pos, spawn_position);
	gfc_vector2d_sub(bounds_pos, bounds_pos, center);
	return bounds_pos;
}

Uint8 spawn_check_bounds_against_world(GFC_Rect bounds) {
	World* world;
	world = world_get_active();

	if (!world) {
		slog("no active world to check bounds against");
		return 0;
	}
	return world_test_collision_rect(world, bounds);
}

Uint8 spawn_check_bounds_against_entities(GFC_Rect bounds) {
	return entity_test_collision_rect(bounds);
}

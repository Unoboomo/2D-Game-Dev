#include "simple_logger.h"
#include "simple_json.h"

#include "camera.h"
#include "entity.h"
#include "spawn.h"

#include "world.h"

extern Uint8 _DRAWBOUNDS; //get external variable, this one is in game.c

static World* active_world = NULL;

void world_tile_layer_build(World* world) {
	GFC_Vector2D position;
	Uint32 frame;
	int i, j;
	Uint32 index;

	if (!world) {
		return;
	}

	if (!world->tileset) {
		slog("cannot draw a world with no tileset");
		return;
	}

	if (world->tile_layer) {
		gf2d_sprite_free(world->tile_layer);
	}
	world->tile_layer = gf2d_sprite_new();

	if (!world->tile_layer) {
		slog("Failed to create new tile_layer sprite");
		return;
	}

	world->tile_layer->frame_w = world->tile_width * world->tileset->frame_w;
	world->tile_layer->frame_h = world->tile_height * world->tileset->frame_h;

	world->tile_layer->surface = gf2d_graphics_create_surface(
		world->tile_layer->frame_w,
		world->tile_layer->frame_h);
	
	SDL_FillRect(world->tile_layer->surface,NULL, SDL_MapRGBA(world->tile_layer->surface->format,0,0,0,0));

	if (!world->tile_layer->surface) {
		slog("failed to create tile_layer surface");
		return;
	}

	for (i = 0; i < world->tile_height; i++) {
		for (j = 0; j < world->tile_width; j++) {

			index = j + (i * world->tile_width);
			if (world->tile_map[index] == 0) {
				continue;
			}
			position.x = j * world->tileset->frame_w;
			position.y = i * world->tileset->frame_h;
			frame = world->tile_map[index] - 1;

			gf2d_sprite_draw_to_surface(
				world->tileset,
				position,
				NULL,
				NULL,
				frame,
				world->tile_layer->surface);
		}
	}

	world->tile_layer->texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(), world->tile_layer->surface);

	if (!world->tile_layer->texture)
	{
		slog("failed to convert world tile layer to texture");
		return;
	}
}

SJson* world_tile_map_save(World* world) {
	SJson* json;
	SJson* tile_map_row;
	int i, j;
	int index;
	int k;

	if (!world || !world->tile_map) {
		slog("cannot save a tilemap that does not exist");
		return NULL;
	}

	json = sj_array_new();

	if (!json) {
		slog("failed to create an array for tilemap");
		return NULL;
	}

	tile_map_row = sj_array_new();

	if (!tile_map_row) {
		slog("failed to create new tile_map_row object");
		sj_free(json);
		return NULL;
	}

	for (i = 0; i < world->tile_height; i++) {
		for (j = 0; j < world->tile_width; j++) {
			index = j + (i * world->tile_width);
			sj_array_append(tile_map_row, sj_new_uint8(world->tile_map[index]));
		}
		
		sj_array_append(json, sj_copy(tile_map_row));
		
		for (k = sj_array_count(tile_map_row) - 1; k >= 0 ; k--) {
			sj_array_delete_nth(tile_map_row, k);
		}
	}

	sj_free(tile_map_row);
	return json;
}

void world_spawn_tile(GFC_Vector2D position, int tile_value) {
	World* world;
	world = world_get_active();
	GFC_Rect bounds;

	if (!world) {
		slog("no active world to spawn an entity to");
		return;
	}
	if (!world->tileset) {
		slog("no way to find tile width and tile height");
		return;
	}

	position.x = (int)position.x / world->tileset->frame_w;
	position.y = (int)position.y / world->tileset->frame_h;

	bounds = gfc_rect(position.x * world->tileset->frame_w, position.y * world->tileset->frame_h, world->tileset->frame_w, world->tileset->frame_h);
	if (entity_test_collision_rect(bounds)) {
		slog("cannot spawn an tile here, it would collide with an entity");
		return;
	}
	world_set_tile_at(world, position, tile_value);
	world_tile_layer_build(world);
}

void world_save(World* world, const char *filename) {
	SJson* json;
	SJson* world_json;
	SJson* tile_map_json;
	if (!filename || !world) {
		return;
	}

	json = sj_object_new();

	if (!json) {
		slog("failed to create new json object");
		return;
	}

	world_json = sj_object_new();

	if (!world_json) {
		slog("failed to create new world_json object");
		sj_free(json);
		return;
	}

	if (world->background) {
		sj_object_insert(world_json, "background", sj_new_str(world->background->filepath));
	}
	if (world->tileset) {
		sj_object_insert(world_json, "tileset", sj_new_str(world->tileset->filepath));
		sj_object_insert(world_json, "frame_w", sj_new_int(world->tileset->frame_w));
		sj_object_insert(world_json, "frame_h", sj_new_int(world->tileset->frame_h));
		sj_object_insert(world_json, "frames_per_line", sj_new_int(world->tileset->frames_per_line));
		tile_map_json = world_tile_map_save(world);
		if (!tile_map_json) {
			slog("failed to save tile map to json");
			sj_free(json);
			sj_free(world_json);
			return;
		}
		sj_object_insert(world_json, "tile_map", tile_map_json);
	}
	sj_object_insert(world_json, "spawn_list", entity_save_all_to_config());
	sj_object_insert(json, "world", world_json);
	sj_save(json, filename);
	sj_free(json);
}	

World* world_load(const char* filename) {
	World* world = NULL;
	SJson* json = NULL;
	SJson* wjson = NULL;
	SJson* vertical;
	SJson* horizontal;
	SJson* item;
	int tile;
	int width = 0;
	int height = 0;
	int i, j;
	const char* background;
	const char* tileset;
	int frame_w, frame_h;
	int frames_per_line;

	if (!filename) {
		slog("no filename provided for world_load");
		return NULL;
	}

	json = sj_load(filename);
	if (!json) {
		slog("failed to load world file %s", filename);
		return NULL;
	}

	wjson = sj_object_get_value(json, "world");
	if (!wjson) {
		slog("%s missing 'world' object",filename);
		sj_free(json);
		return NULL;
	}

	vertical = sj_object_get_value(wjson, "tile_map");
	if (!vertical) {
		slog("%s missing 'tile_map'",filename);
		sj_free(json);
		return NULL;
	}
	height = sj_array_get_count(vertical);

	horizontal = sj_array_get_nth(vertical,0);
	if (!horizontal) {
		slog("%s missing 'tile_map' rows",filename);
		sj_free(json);
		return NULL;
	}
	width = sj_array_get_count(horizontal);

	world = world_new(width,height);
	if (!world) {
		slog("failed to create world with file %s",filename);
		sj_free(json);
		return NULL;
	}


	for (i = 0; i < height; i++) {
		horizontal = sj_array_get_nth(vertical, i);
		if (!horizontal) { // may want to error over this, but for now skip
			continue;
		}

		for (j = 0; j < width; j++) {
			item = sj_array_get_nth(horizontal, j);
			if (!item) {
				continue;
			}
			tile = 0;
			sj_get_integer_value(item, &tile);
			world->tile_map[j + (i * width)] = tile;

		}
	}

	background = sj_object_get_value_as_string(wjson, "background");
	if (!background) {
		slog("%s missing 'tileset'", filename);
		sj_free(json);
		return NULL;
	}

	tileset = sj_object_get_value_as_string(wjson, "tileset");
	if (!tileset) {
		slog("%s missing 'tileset'", filename);
		sj_free(json);
		return NULL;
	}

	sj_object_get_value_as_int(wjson, "frame_w", &frame_w);
	if (!frame_w) {
		slog("%s missing 'frame_w'", filename);
		sj_free(json);
		return NULL;
	}

	sj_object_get_value_as_int(wjson, "frame_h", &frame_h);
	if (!frame_h) {
		slog("%s missing 'frame_h'", filename);
		sj_free(json);
		return NULL;
	}

	sj_object_get_value_as_int(wjson, "frames_per_line", &frames_per_line);
	if (!frames_per_line) {
		slog("%s missing 'frames_per_line'", filename);
		sj_free(json);
		return NULL;
	}

	world->background = gf2d_sprite_load_image(background);
	world->tileset = gf2d_sprite_load_all(
		tileset,
		frame_w,
		frame_h,
		frames_per_line,
		1);
	world_tile_layer_build(world);
	item = sj_object_get_value(wjson, "spawn_list");

	spawn_entities_from_json(item);
	sj_free(json);
	active_world = world;
	world_setup_camera(world);

	return world;
}

World* world_get_active() {
	return active_world;
}

World* world_test_new() {
	int i;
	int width = 21;
	int height = 12;
	World* world;

	world = world_new(width, height);
	if (!world) {
		return NULL;
	}

	world->background = gf2d_sprite_load_image("images/backgrounds/CHEESE.jpg");
	world->tileset = gf2d_sprite_load_all(
		"images/backgrounds/tileset.png",
		60,
		60,
		1,
		1);
	for (i = 0; i < width; i++) {
		world->tile_map[i] = 1;
		world->tile_map[i + ((height - 1) * width)] = 1;
	}
	for (i = 0; i < height; i++) {
		world->tile_map[i * width] = 1;
		world->tile_map[i * width + width - 1] = 1;
	}

	world_tile_layer_build(world);
	return world;
}

World* world_new(Uint32 width, Uint32 height) {
	World* world;
	if (!width || !height) {
		slog("cannot make a world with zero width or height");
		return NULL;
	}
	world = gfc_allocate_array(sizeof(World), 1);
	if (!world) {
		slog("failed to allocate a new world");
		return NULL;
	}
	//defaults and stuff would go here
	world->tile_map = gfc_allocate_array(sizeof(Uint8), height * width);
	world->tile_height = height;
	world->tile_width = width;
	return world;
}

void world_free(World* world) {
	if (!world) {
		return;
	}
	gf2d_sprite_free(world->background);
	gf2d_sprite_free(world->tileset);
	gf2d_sprite_free(world->tile_layer);
	free(world->tile_map);
	free(world);
}

Uint8 world_get_tile_at(World* world, GFC_Vector2D position) {
	if (!world || !world->tile_map) {
		slog("cannot get a tile in world or tile_map that does not exist");
		return 0;
	}
	return world->tile_map[(Uint32)position.y * (Uint32)world->tile_width + (Uint32)position.x];
}

void world_set_tile_at(World* world, GFC_Vector2D position, int tile_value) {
	if (!world || !world->tile_map) {
		slog("cannot set a tile in world or tile_map that does not exist");
		return;
	}

	world->tile_map[(Uint32)position.y * (Uint32)world->tile_width + (Uint32)position.x] = tile_value;
}

void world_draw_background(World* world) {
	if (!world) {
		slog("cannot draw a world that doesn't exist");
		return;
	}
	if (!world->background) {
		slog("cannot draw a world with no background");
	}
	gf2d_sprite_draw_image(world->background, gfc_vector2d(0, 0));

}

void world_draw_tile_layer(World* world) {
	GFC_Vector2D offset;
	int i, j;
	int index;
	if (!world) {
		slog("cannot draw a world that doesn't exist");
		return;
	}
	if (!world->tile_layer) {
		slog("cannot draw a world with no tile_layer");
	}

	offset = camera_get_offset();

	gf2d_sprite_draw_image(world->tile_layer, offset);

	if (_DRAWBOUNDS) {
		if (!world->tile_map) {
			return;
		}
		for (i = 0; i < world->tile_height; i++) {
			for (j = 0; j < world->tile_width; j++) {

				index = j + (i * world->tile_width);
				if (world->tile_map[index] == 0) {
					continue;
				}
				offset.x = j * world->tileset->frame_w;
				offset.y = i * world->tileset->frame_h;

				gfc_vector2d_add(offset, offset, camera_get_offset());

				gf2d_draw_rect(gfc_rect(offset.x, offset.y, world->tileset->frame_w, world->tileset->frame_h), GFC_COLOR_YELLOW);
			}
		}
	}
}

void world_setup_camera(World* world) {
	if (!world) {
		slog("no world to setup camera");
		return;
	}
	if (!world->tile_layer || !world->tile_layer->surface) {
		slog("no tile layer set for world");
		return;
	}
	camera_set_bounds(gfc_rect(0, 0, world->tile_layer->surface->w, world->tile_layer->surface->h));
	camera_apply_bounds();
	camera_enable_binding(1);
}

Uint8 world_test_collision_rect(World* world, GFC_Rect bounds) {
	GFC_Rect tile_rect = { 0 };
	Uint8 tile_index;
	int i, j;

	if (!world || !world->tileset) {
		return 0;
	}

	tile_rect.w = world->tileset->frame_w;
	tile_rect.h = world->tileset->frame_h;

	for (i = 0; i < world->tile_height; i++) {
		for (j = 0; j < world->tile_width; j++) {
			tile_index = world_get_tile_at(world, gfc_vector2d(j, i));

			if (!tile_index) {
				continue; //tile is empty/air
			}

			tile_rect.x = j * tile_rect.w;
			tile_rect.y = i * tile_rect.h;
			if (gfc_rect_overlap(tile_rect, bounds)) {
				return 1;
			}
		}
	}
	return 0;
}

void world_switch_to_next() {
	Entity* player;
	world_free(active_world);
	world_load("worlds/testworld.world");
	entity_system_free_all_but_player();
	player = entity_get_player();
	player->physics->position = gfc_vector2d(20, 20);
}
#include "simple_logger.h"
#include "simple_json.h"

#include "world.h"

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

	sj_free(json);

	return world;
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
	slog("allocate a new world");
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

void world_draw(World* world) {
	if (!world) {
		slog("cannot draw a world that doesn't exist");
		return;
	}
	if (!world->background) {
		slog("cannot draw a world with no background");
	}
	if (!world->tile_layer) {
		slog("cannot draw a world with no tile_layer");
	}

	gf2d_sprite_draw_image(world->background, gfc_vector2d(0, 0));
	gf2d_sprite_draw_image(world->tile_layer, gfc_vector2d(0, 0));
}
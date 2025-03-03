#include "simple_logger.h"

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
World* world_test_new() {
	int i;
	int width = 75;
	int height = 45;
	World* world;

	world = world_new(width, height);
	if (!world) {
		return NULL;
	}

	world->background = gf2d_sprite_load_image("images/backgrounds/CHEESE.jpg");
	world->tileset = gf2d_sprite_load_all(
		"images/backgrounds/tileset.png",
		16,
		16,
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
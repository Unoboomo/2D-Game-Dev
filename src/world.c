#include "simple_logger.h"

#include "world.h"

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
	free(world->tile_map);
}

void world_draw(World* world) {
	int i, j;
	int frame;
	int index;
	GFC_Vector2D position;
	if (!world) {
		slog("cannot draw a world that doesn't exist");
		return;
	}
	if (!world->tileset) {
		slog("cannot draw a world with no tileset");
		return;
	}
	gf2d_sprite_draw_image(world->background, gfc_vector2d(0, 0));
	for (i = 0; i < world->tile_height; i++) {
		for (j = 0; j < world->tile_width; j++) {
			index = j + (i * world->tile_width);
			if (world->tile_map[index] == 0) {
				continue;
			}
			position.x = j * world->tileset->frame_w;
			position.y = i * world->tileset->frame_h;
			frame = world->tile_map[index] - 1;
			gf2d_sprite_draw(
				world->tileset,
				position,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				frame);
		}
	}
}
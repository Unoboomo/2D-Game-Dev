#include "simple_logger.h"

#include "gfc_config.h"
#include "gf2d_draw.h"

#include "particle.h"

typedef struct {
	Particle*	particle_list;
	Uint32		particle_count;
}ParticleManager;

static ParticleManager particle_manager = { 0 };

void particle_system_close() {
	int i;
	if (particle_manager.particle_list) {
		for (i = 0; i < particle_manager.particle_count; i++) {
			particle_free(&particle_manager.particle_list[i]);
		}
		free(particle_manager.particle_list);
		particle_manager.particle_list = NULL;
		particle_manager.particle_count = 0;
	}
}

void particle_system_init(Uint32 max_particles) {
	if (!max_particles) {
		slog("cannot initialioze particle system for zero particles");
		return;
	}
	if (particle_manager.particle_list) {
		slog("particle manager already initialized");
		return;
	}
	particle_manager.particle_list = gfc_allocate_array(sizeof(Particle), max_particles);
	if (!particle_manager.particle_list) {
		slog("failed to allocate particles for the manager");
		return;
	}
	particle_manager.particle_count = max_particles;
	atexit(particle_system_close);
	slog("particle manager initialized");
}

void particle_system_clear() {
	int i;
	if (!particle_manager.particle_list) {
		return;
	}
	for (i = 0; i < particle_manager.particle_count; i++) {

		particle_free(&particle_manager.particle_list[i]);
	}
	memset(particle_manager.particle_list, 0, sizeof(Particle) * particle_manager.particle_count);
}

void particle_draw(Particle* particle) {
	if (!particle) {
		return;
	}
	particle->ttl--;
	if (particle->ttl <= 0) {
		particle_free(particle);
		particle->_inuse = 0;
		return;
	}

	gfc_vector2d_add(particle->velocity, particle->velocity, particle->acceleration);
	gfc_vector2d_add(particle->position, particle->position, particle->velocity);

	gfc_color_add(&particle->color, particle->color, particle->color_velocity); //update color over time

	if (particle->p_type == PT_Sprite) {
		particle->frame += particle->frame_rate;

		if (particle->frame > particle->frame_end) {
			if (particle->loops) {
				particle->frame = particle->frame_start;
			}
			else {
				particle->frame = particle->frame_end;
				particle->frame_rate = 0;
			}
		}
	}

	switch (particle->p_type) {
 		case PT_Point:
			gf2d_draw_pixel(particle->position, particle->color);
			break;
		case PT_Shape:
			gf2d_draw_shape(particle->shape, particle->color, particle->position);
			break;
		case PT_Sprite:
			gf2d_sprite_draw(
				particle->sprite,
				particle->position,
				NULL,
				NULL,
				NULL,
				NULL,
				&particle->color,
				(Uint32) particle->frame);
			break;
	}
}

void particle_system_draw() {
	int i;
	for (i = 0; i < particle_manager.particle_count; i++) {

		if (!particle_manager.particle_list[i]._inuse) {
			continue;
		}
		particle_draw(&particle_manager.particle_list[i]);
	}
}

Particle* particle_new() {
	int i;
	for (i = 0; i < particle_manager.particle_count; i++) {

		if (!particle_manager.particle_list[i]._inuse) {

			memset(&particle_manager.particle_list[i], 0, sizeof(Particle));
			particle_manager.particle_list[i]._inuse = 1;
			return &particle_manager.particle_list[i];
		}
	}
	slog("no space for new particles");
	return NULL;
}

void particle_free(Particle* particle) {
	if (!particle) {
		slog("cannot free a particle that does not exist");
		return;
	}
	if (particle->sprite) {
		gf2d_sprite_free(particle->sprite);
	}
}

void particles_from_file(const char* filename, Uint32 count, GFC_Vector2D position, GFC_Vector2D direction, GFC_Vector2D acceleration) {
	int i;
	SJson* json;
	json = sj_load(filename);
	if (!json) {
		return;
	}
	for (i = 0; i < count; i++) {
		particle_from_json(json,position,direction,acceleration);
	}
	sj_free(json);
}

Particle* particle_from_json(SJson* json, GFC_Vector2D position, GFC_Vector2D direction, GFC_Vector2D acceleration) {
	int particle_type = 0;
	Uint32 ttl_variance = 0;
	float speed = 0;
	float speed_variance = 0;
	float angle_variance = 0;
	GFC_Color color_variance = { 0 };

	int shape_type = 0;
	GFC_Vector4D shape_4d = { 0 };
	GFC_Vector3D shape_3d = { 0 };

	GFC_Vector2D frame_size = { 0 };
	Uint32 frames_per_line = 0;
	const char* sprite = NULL;

	Uint8 loops = 0;

	Particle* p;
	if (!json) {
		return NULL;
	}
	p = particle_new();
	if (!p) {
		slog("failed to create a new particle");
		return NULL;
	}

	sprite = sj_object_get_string(json, "name");
	if (sprite) {
		gfc_line_cpy(p->name, sprite);
	}
	sprite = NULL;

	sj_object_get_int(json, "particle_type", &particle_type);
	p->p_type = (ParticleType)particle_type;

	sj_object_get_uint32(json, "ttl", &p->ttl);

	if (sj_object_get_uint32(json, "ttl_variance", &ttl_variance)) {
		p->ttl += ttl_variance * gfc_crandom();
	}

	gfc_vector2d_copy(p->position, position);
	gfc_vector2d_normalize(&direction);
	
	sj_object_get_float(json, "angle_variance", &angle_variance);
	angle_variance *= GFC_DEGTORAD;
	angle_variance *= gfc_crandom();
	direction = gfc_vector2d_rotate(direction, angle_variance);
	gfc_vector2d_copy(p->velocity, direction);


	sj_object_get_float(json, "speed", &speed);
	sj_object_get_float(json, "speed_variance", &speed_variance);
	speed += gfc_crandom() * speed_variance; //positive values only
	gfc_vector2d_scale(p->velocity, p->velocity, speed);
	gfc_vector2d_copy(p->acceleration, acceleration);


	sj_object_get_color_value(json, "color",&p->color);
	sj_object_get_color_value(json, "color_velocity", &p->color_velocity);
	if (sj_object_get_color_value(json, "color_variance", &color_variance)) {
		color_variance.r *= gfc_crandom();
		color_variance.g *= gfc_crandom();
		color_variance.b *= gfc_crandom();
		color_variance.a *= gfc_crandom();
		gfc_color_add(&p->color, p->color, color_variance);
	}
	gfc_color_clamp(p->color);

	if (p->p_type == PT_Shape) {
		sj_object_get_int(json, "shape_type", &shape_type);
		p->shape.type = (GFC_ShapeTypes)shape_type;
		if (p->shape.type == ST_RECT) {
			sj_object_get_vector4d(json, "shape", &shape_4d);
			p->shape = gfc_shape_rect(shape_4d.x, shape_4d.y, shape_4d.z, shape_4d.w);
		}
		else if (p->shape.type == ST_EDGE) {
			sj_object_get_vector4d(json, "shape", &shape_4d);
			p->shape = gfc_shape_edge(shape_4d.x, shape_4d.y, shape_4d.z,shape_4d.w);
		}
		else if (p->shape.type == ST_CIRCLE) {
			sj_object_get_vector3d(json, "shape", &shape_3d);
			p->shape = gfc_shape_circle(shape_3d.x, shape_3d.y, shape_3d.z);
		}
		else {
			slog("invalid shape_type");
		}
	}
	else if (p->p_type == PT_Sprite) {
		sprite = sj_object_get_string(json, "sprite");
		if (sprite) {
			sj_object_get_vector2d(json, "sprite_size", &frame_size);
			sj_object_get_uint32(json, "sprite_fpl", &frames_per_line);
			p->sprite = gf2d_sprite_load_all(
				sprite,
				(Uint32)frame_size.x,
				(Uint32)frame_size.y,
				frames_per_line,
				0);

			sj_object_get_float(json, "frame_rate", &speed);
			p->frame_rate = speed;

			sj_object_get_int(json, "frame_start", &shape_type);
			p->frame_start = shape_type;
			p->frame = (float) p->frame_start;

			sj_object_get_int(json, "frame_end", &shape_type);
			p->frame_end = shape_type;

			sj_object_get_uint8(json, "loops", &loops);
			p->loops = loops;
		}
		else {
			slog("invalide sprite");
		}
	}

	return p;
}

#ifndef PARTICLE_H
#define PARTICLE_H

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdbool.h>
#include "shader.h"

#define MAX_PARTICLES 5000 

typedef struct {
    vec3 position;
    vec3 velocity;
    vec4 color;
    float life;
    float max_life;
    float size;
} Particle;

typedef struct {
    vec3 position;
    vec4 color;
    float size;
} ParticleRenderData;

typedef struct {
    Particle particles[MAX_PARTICLES];
    ParticleRenderData render_data[MAX_PARTICLES];
    int last_used_particle;

    GLuint vao;
    GLuint vbo_quad;
    GLuint vbo_instance;

    Shader shader;
    GLuint texture_id;
} ParticleSystem;

void init_particle_system(ParticleSystem* ps, const char* texture_path);
void emit_particle(ParticleSystem* ps, vec3 pos, vec3 vel, vec4 color, float life, float size);
void update_particles(ParticleSystem* ps, double dt);
void render_particles(ParticleSystem* ps, mat4 view, mat4 proj);
void clear_particles(ParticleSystem* ps);
void destroy_particle_system(ParticleSystem* ps);

#endif

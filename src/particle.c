#include "particle.h"
#include "texture.h"

#include <glad/glad.h>

#include <stdlib.h>
#include <string.h>

static int first_unused_particle(ParticleSystem* ps);
static void setup_particle_buffers(ParticleSystem* ps);

void init_particle_system(ParticleSystem* ps, const char* texture_path) {
    ps->last_used_particle = 0;

    for (int i = 0; i < MAX_PARTICLES; i++) {
        ps->particles[i].life = 0.0f; 
    }

    load_shader(&ps->shader, "assets/shaders/particle.vert", "assets/shaders/particle.frag");
    ps->texture_id = get_texture(texture_path);

    setup_particle_buffers(ps);
}

void emit_particle(ParticleSystem* ps, vec3 pos, vec3 vel, vec4 color, float life, float size) {
    int id = first_unused_particle(ps);

    glm_vec3_copy(pos, ps->particles[id].position);
    glm_vec3_copy(vel, ps->particles[id].velocity);
    glm_vec4_copy(color, ps->particles[id].color);

    ps->particles[id].life = life;
    ps->particles[id].max_life = life;
    ps->particles[id].size = size;
}

void update_particles(ParticleSystem* ps, double dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &ps->particles[i];
        if (p->life > 0.0f) {
            p->life -= (float)dt;
            if (p->life > 0.0f) {
                p->position[0] += p->velocity[0] * (float)dt;
                p->position[1] += p->velocity[1] * (float)dt;
                p->position[2] += p->velocity[2] * (float)dt;
                
                p->color[3] = p->life / p->max_life; 
            }
        }
    }
}

void render_particles(ParticleSystem* ps, mat4 view, mat4 proj) {
    int active_count = 0;

    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (ps->particles[i].life > 0.0f) {
            glm_vec3_copy(ps->particles[i].position, ps->render_data[active_count].position);
            glm_vec4_copy(ps->particles[i].color, ps->render_data[active_count].color);

            ps->render_data[active_count].size = ps->particles[i].size;

            active_count++;
        }
    }

    if (active_count == 0) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, ps->vbo_instance);
    glBufferSubData(GL_ARRAY_BUFFER, 0, active_count * sizeof(ParticleRenderData), ps->render_data);

    use_shader(&ps->shader);

    vec3 cameraRight = {view[0][0], view[1][0], view[2][0]};
    vec3 cameraUp = {view[0][1], view[1][1], view[2][1]};

    glUniformMatrix4fv(glGetUniformLocation(ps->shader.id, "projection"), 1, GL_FALSE, (float*)proj);
    glUniformMatrix4fv(glGetUniformLocation(ps->shader.id, "view"), 1, GL_FALSE, (float*)view);
    glUniform3f(glGetUniformLocation(ps->shader.id, "cameraRight"), cameraRight[0], cameraRight[1], cameraRight[2]);
    glUniform3f(glGetUniformLocation(ps->shader.id, "cameraUp"), cameraUp[0], cameraUp[1], cameraUp[2]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ps->texture_id);
    glUniform1i(glGetUniformLocation(ps->shader.id, "sprite"), 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    glDepthMask(GL_FALSE);             

    glBindVertexArray(ps->vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, active_count);
    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
}

void clear_particles(ParticleSystem* ps) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        ps->particles[i].life = 0.0f;
    }

    ps->last_used_particle = 0; 
}

void destroy_particle_system(ParticleSystem* ps) {
    glDeleteVertexArrays(1, &ps->vao);
    glDeleteBuffers(1, &ps->vbo_quad);
    glDeleteBuffers(1, &ps->vbo_instance);

    destroy_shader(&ps->shader);
}

static int first_unused_particle(ParticleSystem* ps) {
    for (int i = ps->last_used_particle; i < MAX_PARTICLES; i++) {
        if (ps->particles[i].life <= 0.0f) {
            ps->last_used_particle = i;
            return i;
        }
    }

    for (int i = 0; i < ps->last_used_particle; i++) {
        if (ps->particles[i].life <= 0.0f) {
            ps->last_used_particle = i;
            return i;
        }
    }

    return 0; 
}

static void setup_particle_buffers(ParticleSystem* ps) {
    static const float quad_vertices[] = {
        -0.5f,  0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f,

        -0.5f,  0.5f,
        0.5f,  0.5f,
        0.5f, -0.5f
    };

    glGenVertexArrays(1, &ps->vao);
    glGenBuffers(1, &ps->vbo_quad);
    glGenBuffers(1, &ps->vbo_instance);

    glBindVertexArray(ps->vao);

    glBindBuffer(GL_ARRAY_BUFFER, ps->vbo_quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, ps->vbo_instance);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(ParticleRenderData), NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleRenderData), (void*)offsetof(ParticleRenderData, position));
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleRenderData), (void*)offsetof(ParticleRenderData, color));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleRenderData), (void*)offsetof(ParticleRenderData, size));
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
}

#ifndef MODEL_H
#define MODEL_H

#include "utils.h"

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdbool.h>

typedef struct {
    vec3_t position;
    vec3_t normal;
    float u, v;
} Vertex;

typedef struct {
    Vertex* vertices;
    int vertex_count;
    GLuint vao, vbo;
    vec3 base_extents; 
    vec3 base_offset;
} Model;

void init_model_cache();
Model* get_model(const char* filename);
void free_model_cache();
void draw_model(const Model* model);

#endif

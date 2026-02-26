#ifndef MODEL_H
#define MODEL_H

#include "utils.h"
#include <stdbool.h>

typedef struct Vertex {
    vec3 position;
    vec3 normal;
    float u, v;
} Vertex;

typedef struct Model {
    Vertex* vertices;
    int vertex_count;
} Model;

void init_model_cache();

Model* get_model(const char* filename);

void free_model_cache();

bool load_model(Model* model, const char* filename);

void free_model(Model* model);

void draw_model(const Model* model);

#endif

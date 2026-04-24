#ifndef MODEL_H
#define MODEL_H

#include "utils.h"

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdbool.h>

#define MAX_SUBMESHES 64

typedef struct {
    vec3 position;
    vec3 normal;
    float u, v;
} Vertex;

typedef struct {
    int start_vertex;
    int vertex_count;
    int material_id;
} SubMesh;

typedef struct {
    Vertex* vertices;
    int vertex_count;
    GLuint vao, vbo;
    vec3 base_extents; 
    vec3 base_offset;
    
    SubMesh submeshes[MAX_SUBMESHES];
    int submesh_count;
} Model;

void init_model_cache();
Model* get_model(const char* filename);
void free_model_cache();
void draw_model(const Model* model);
void draw_model_submesh(const Model* model, int submesh_index);
void clear_model_cache();

#endif

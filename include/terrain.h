#ifndef TERRAIN_H
#define TERRAIN_H

#include "shader.h"
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdbool.h>

typedef struct {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} TerrainVertex;

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    
    int vertex_count;
    int index_count;
    
    int width;
    int length;
    
    float* heights;
    
    float scale_xy;
    float scale_z;
    
    GLuint texture_id;
} Terrain;

bool init_terrain(Terrain* terrain, const char* heightmap_path, const char* texture_path, float scale_xy, float scale_z, float tile_size);

void render_terrain(const Terrain* terrain, Shader* shader);

float get_terrain_height(const Terrain* terrain, float world_x, float world_z);

void destroy_terrain(Terrain* terrain);

#endif
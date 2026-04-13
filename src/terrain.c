#include "terrain.h"
#include "texture.h"

#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static float get_vertex_height(Terrain* terrain, int x, int y) {
    if (x < 0 || x >= terrain->width || y < 0 || y >= terrain->length) return 0.0f;
    return terrain->heights[y * terrain->width + x];
}

bool init_terrain(Terrain* terrain, const char* heightmap_path, const char* texture_path, float scale_xy, float scale_z, float tile_size) {
    SDL_Surface* raw_surface = IMG_Load(heightmap_path);
    if (!raw_surface) {
        printf("ERROR: Failed to load heightmap: %s\n", heightmap_path);
        
        return false;
    }

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGB24, 0);
    SDL_FreeSurface(raw_surface);

    if (!surface) {
        return false;
    }

    terrain->width = surface->w;
    terrain->length = surface->h;
    terrain->scale_xy = scale_xy;
    terrain->scale_z = scale_z;

    terrain->heights = (float*)malloc(terrain->width * terrain->length * sizeof(float));

    terrain->vertex_count = terrain->width * terrain->length;
    TerrainVertex* vertices = (TerrainVertex*)malloc(terrain->vertex_count * sizeof(TerrainVertex));

    Uint8* pixels = (Uint8*)surface->pixels;
    int bytes_per_row = surface->pitch;

    if (tile_size <= 0.0f) {
        float total_world_width = terrain->width * scale_xy;
        tile_size = total_world_width / 15.0f;
    }

    for (int y = 0; y < terrain->length; y++) {
        for (int x = 0; x < terrain->width; x++) {
            Uint8 r = pixels[y * bytes_per_row + x * 3]; 
            
            float normalized_height = (float)r / 255.0f;
            float height = normalized_height * scale_z;
            
            terrain->heights[y * terrain->width + x] = height;

            int index = y * terrain->width + x;
            float world_x = x * scale_xy;
            float world_y = y * scale_xy;

            vertices[index].position[0] = world_x;
            vertices[index].position[1] = world_y;
            vertices[index].position[2] = height;

            vertices[index].texCoords[0] = world_x / tile_size; 
            vertices[index].texCoords[1] = world_y / tile_size;
        }
    }
    SDL_FreeSurface(surface);

    for (int y = 0; y < terrain->length; y++) {
        for (int x = 0; x < terrain->width; x++) {
            float hl = get_vertex_height(terrain, x - 1, y);
            float hr = get_vertex_height(terrain, x + 1, y);
            float hd = get_vertex_height(terrain, x, y - 1);
            float hu = get_vertex_height(terrain, x, y + 1);

            vec3 normal = {hl - hr, hd - hu, 2.0f * scale_xy};
            glm_vec3_normalize(normal);

            int index = y * terrain->width + x;
            glm_vec3_copy(normal, vertices[index].normal);
        }
    }

    terrain->index_count = (terrain->width - 1) * (terrain->length - 1) * 6;
    GLuint* indices = (GLuint*)malloc(terrain->index_count * sizeof(GLuint));
    
    int i = 0;
    for (int y = 0; y < terrain->length - 1; y++) {
        for (int x = 0; x < terrain->width - 1; x++) {
            int top_left = y * terrain->width + x;
            int top_right = top_left + 1;
            int bottom_left = (y + 1) * terrain->width + x;
            int bottom_right = bottom_left + 1;

            indices[i++] = top_left;
            indices[i++] = bottom_left;
            indices[i++] = top_right;

            indices[i++] = top_right;
            indices[i++] = bottom_left;
            indices[i++] = bottom_right;
        }
    }

    terrain->texture_id = get_texture(texture_path);

    glGenVertexArrays(1, &terrain->vao);
    glGenBuffers(1, &terrain->vbo);
    glGenBuffers(1, &terrain->ebo);

    glBindVertexArray(terrain->vao);

    glBindBuffer(GL_ARRAY_BUFFER, terrain->vbo);
    glBufferData(GL_ARRAY_BUFFER, terrain->vertex_count * sizeof(TerrainVertex), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrain->index_count * sizeof(GLuint), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(3 * sizeof(float)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    free(vertices);
    free(indices);

    return true;
}

void render_terrain(const Terrain* terrain, Shader* shader) {
    if (terrain->vao == 0) {
        return;
    }

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, (float*)model);

    glUniform3f(glGetUniformLocation(shader->id, "objScale"), 1.0f, 1.0f, 1.0f);
    
    glUniform3f(glGetUniformLocation(shader->id, "specularColor"), 0.1f, 0.1f, 0.1f);
    glUniform1f(glGetUniformLocation(shader->id, "shininess"), 10.0f);
    glUniform1i(glGetUniformLocation(shader->id, "u_has_glow"), 0);

    glUniform2f(glGetUniformLocation(shader->id, "u_uv_speed"), 0.0f, 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrain->texture_id);
    glUniform1i(glGetUniformLocation(shader->id, "texture_diffuse"), 0);

    glBindVertexArray(terrain->vao);
    glDrawElements(GL_TRIANGLES, terrain->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

float get_terrain_height(const Terrain* terrain, float world_x, float world_y) {
    if (!terrain || !terrain->heights) {
        return 0.0f;
    }

    int grid_x = (int)floor(world_x / terrain->scale_xy);
    int grid_y = (int)floor(world_y / terrain->scale_xy);

    if (grid_x < 0 || grid_x >= terrain->width - 1 || 
        grid_y < 0 || grid_y >= terrain->length - 1) {
        return -1e10f;
    }

    float tx = (world_x - (grid_x * terrain->scale_xy)) / terrain->scale_xy;
    float ty = (world_y - (grid_y * terrain->scale_xy)) / terrain->scale_xy;

    float h00 = terrain->heights[grid_y * terrain->width + grid_x];         
    float h10 = terrain->heights[grid_y * terrain->width + (grid_x + 1)];     
    float h01 = terrain->heights[(grid_y + 1) * terrain->width + grid_x];     
    float h11 = terrain->heights[(grid_y + 1) * terrain->width + (grid_x + 1)];

    float h_top = h00 * (1.0f - tx) + h10 * tx;
    float h_bottom = h01 * (1.0f - tx) + h11 * tx;

    return h_top * (1.0f - ty) + h_bottom * ty;
}

void destroy_terrain(Terrain* terrain) {
    if (terrain->heights) {
        free(terrain->heights);
        terrain->heights = NULL;
    }
    
    if (terrain->vao) {
        glDeleteVertexArrays(1, &terrain->vao);
    }
    if (terrain->vbo) {
        glDeleteBuffers(1, &terrain->vbo);
    }
    if (terrain->ebo) {
        glDeleteBuffers(1, &terrain->ebo);
    }
}

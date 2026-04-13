#include "texture.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <string.h>

#define MAX_TEXTURES 100

typedef struct {
    char filename[256];
    GLuint id;
} CachedTexture;

static CachedTexture texture_cache[MAX_TEXTURES];
static int texture_count = 0;

static GLuint load_texture(const char* filename);

void init_texture_cache() {
    texture_count = 0;
}

GLuint get_texture(const char* filename) {
    for (int i = 0; i < texture_count; i++) {
        if (strcmp(texture_cache[i].filename, filename) == 0) {
            return texture_cache[i].id;
        }
    }

    if (texture_count < MAX_TEXTURES) {
        GLuint id = load_texture(filename);
        if (id != 0) {
            strncpy(texture_cache[texture_count].filename, filename, sizeof(texture_cache[texture_count].filename) - 1);
            texture_cache[texture_count].filename[sizeof(texture_cache[texture_count].filename) - 1] = '\0';

            texture_cache[texture_count].id = id;
            texture_count++;

            return id;
        }
    } else {
        printf("ERROR: Max textures reached. (%d)\n", MAX_TEXTURES);
    }

    return 0;
}

void free_texture_cache() {
    for (int i = 0; i < texture_count; i++) {
        glDeleteTextures(1, &(texture_cache[i].id));
    }
    texture_count = 0;
}

GLuint load_cubemap(const char* faces[6]) {
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    for (int i = 0; i < 6; i++) {
        SDL_Surface* raw_surface = IMG_Load(faces[i]);
        if (!raw_surface) {
            printf("ERROR: Failed to load skybox: %s\n", faces[i]);
            continue;
        }
        
        SDL_Surface* surface = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(raw_surface);

        if (surface) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
            SDL_FreeSurface(surface);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture_id;
}

static GLuint load_texture(const char* filename) {
    SDL_Surface* raw_surface = IMG_Load(filename);
    if (!raw_surface) {
        printf("ERROR: Failed to load texture: %s\n", filename);

        return 0;
    }

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(raw_surface);

    if (!surface) {
        printf("ERROR: Failed to convert texture format: %s\n", filename);

        return 0;
    }

    GLuint texture_name;
    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
    
    return texture_name;
}

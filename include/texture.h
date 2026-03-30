#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

void init_texture_cache();
GLuint get_texture(const char* filename);
void free_texture_cache();
GLuint load_cubemap(const char* faces[6]);

#endif
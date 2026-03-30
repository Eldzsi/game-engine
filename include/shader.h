#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <stdbool.h>

typedef struct {
    GLuint id;
} Shader;

bool load_shader(Shader* shader, const char* vertex_path, const char* fragment_path);
void use_shader(const Shader* shader);
void destroy_shader(Shader* shader);

#endif

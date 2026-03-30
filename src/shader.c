#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* read_file_to_string(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        printf("ERROR: Failed to open shader file: %s\n", path);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        printf("ERROR: Memory allocation failed for shader: %s\n", path);
        fclose(file);
        return NULL;
    }
    
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char**)&source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("ERROR: Shader compilation failed (%d):\n%s\n", type, info_log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool load_shader(Shader* shader, const char* vertex_path, const char* fragment_path) {
    char* vertex_source = read_file_to_string(vertex_path);
    char* fragment_source = read_file_to_string(fragment_path);

    if (!vertex_source || !fragment_source) {
        if (vertex_source) free(vertex_source);
        if (fragment_source) free(fragment_source);
        return false;
    }

    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

    free(vertex_source);
    free(fragment_source);

    if (vertex_shader == 0 || fragment_shader == 0) {
        if (vertex_shader) glDeleteShader(vertex_shader);
        if (fragment_shader) glDeleteShader(fragment_shader);
        return false;
    }

    shader->id = glCreateProgram();
    glAttachShader(shader->id, vertex_shader);
    glAttachShader(shader->id, fragment_shader);
    glLinkProgram(shader->id);

    int success;
    glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader->id, 512, NULL, info_log);
        printf("ERROR: Shader linking failed:\n%s\n", info_log);
        glDeleteProgram(shader->id);
        shader->id = 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return success;
}

void use_shader(const Shader* shader) {
    if (shader && shader->id != 0) {
        glUseProgram(shader->id);
    }
}

void destroy_shader(Shader* shader) {
    if (shader && shader->id != 0) {
        glDeleteProgram(shader->id);
        shader->id = 0;
    }
}

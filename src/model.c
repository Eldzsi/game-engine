#define FAST_OBJ_IMPLEMENTATION

#include "model.h"

#include "fast_obj.h"

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MODELS 20

typedef struct {
    char filename[256];
    Model model;
} CachedModel;

static CachedModel model_cache[MAX_MODELS];
static int model_count = 0;

static bool load_model(Model* model, const char* filename);
static void free_model(Model* model);
static void setup_buffers(Model* model);
static void calculate_model_extents(Model* model, vec3 min_p, vec3 max_p);

void init_model_cache() {
    model_count = 0;
}

Model* get_model(const char* filename) {
    for (int i = 0; i < model_count; i++) {
        if (strcmp(model_cache[i].filename, filename) == 0) {
            return &(model_cache[i].model);
        }
    }

    if (model_count < MAX_MODELS) {
        if (load_model(&(model_cache[model_count].model), filename)) {
            strncpy(model_cache[model_count].filename, filename, sizeof(model_cache[model_count].filename) - 1);
            model_cache[model_count].filename[sizeof(model_cache[model_count].filename) - 1] = '\0';

            Model* loaded_model = &(model_cache[model_count].model);
            model_count++;

            return loaded_model;
        }
    } else {
        printf("ERROR: Max models reached.\n");
    }

    return NULL;
}

void free_model_cache() {
    for (int i = 0; i < model_count; i++) {
        free_model(&(model_cache[i].model));
    }

    model_count = 0;
}

void draw_model(const Model* model) {
    if (model->vao == 0) {
        return;
    }

    glBindVertexArray(model->vao);
    glDrawArrays(GL_TRIANGLES, 0, model->vertex_count);
    glBindVertexArray(0);
}

static void setup_buffers(Model* model) {
    glGenVertexArrays(1, &model->vao);
    glBindVertexArray(model->vao);

    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, model->vertex_count * sizeof(Vertex), model->vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

static void calculate_model_extents(Model* model, vec3 min_p, vec3 max_p) {
    model->base_extents[0] = max_p[0] - min_p[0];
    model->base_extents[1] = max_p[1] - min_p[1];
    model->base_extents[2] = max_p[2] - min_p[2];

    model->base_offset[0] = (min_p[0] + max_p[0]) / 2.0f;
    model->base_offset[1] = (min_p[1] + max_p[1]) / 2.0f;
    model->base_offset[2] = (min_p[2] + max_p[2]) / 2.0f;
}

static bool load_model(Model* model, const char* filename) {
    fastObjMesh* mesh = fast_obj_read(filename);
    if (!mesh) {
        printf("ERROR: Failed to load object: %s\n", filename);

        return false;
    }

    model->vertex_count = mesh->face_count * 3;
    model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
    if (model->vertices == NULL) {
        printf("ERROR: Failed to load model.\n");
        fast_obj_destroy(mesh);

        return false;
    }

    vec3 min_p;
    min_p[0] = 1e10f;
    min_p[1] = 1e10f;
    min_p[2] = 1e10f;

    vec3 max_p;
    max_p[0] = -1e10f;
    max_p[1] = -1e10f;
    max_p[2] = -1e10f;

    for (int i = 0; i < model->vertex_count; i++) {
        fastObjIndex index = mesh->indices[i];
        
        float px = mesh->positions[index.p * 3 + 0];
        float py = mesh->positions[index.p * 3 + 1];
        float pz = mesh->positions[index.p * 3 + 2];

        model->vertices[i].position[0] = px;
        model->vertices[i].position[1] = py;
        model->vertices[i].position[2] = pz;

        if (px < min_p[0]) {
            min_p[0] = px;
        }
        if (px > max_p[0]) {
            max_p[0] = px;
        }
        if (py < min_p[1]) {
            min_p[1] = py;
        }
        if (py > max_p[1]) {
            max_p[1] = py;
        }
        if (pz < min_p[2]) {
            min_p[2] = pz;
        }
        if (pz > max_p[2]) {
            max_p[2] = pz;
        }

        if (mesh->normal_count > 0) {
            model->vertices[i].normal[0] = mesh->normals[index.n * 3 + 0];
            model->vertices[i].normal[1] = mesh->normals[index.n * 3 + 1];
            model->vertices[i].normal[2] = mesh->normals[index.n * 3 + 2];
        }

        if (mesh->texcoord_count > 0) {
            model->vertices[i].u = mesh->texcoords[index.t * 2 + 0];
            model->vertices[i].v = mesh->texcoords[index.t * 2 + 1];
        }
    }

    calculate_model_extents(model, min_p, max_p);
    setup_buffers(model);

    fast_obj_destroy(mesh);
    return true;
}

static void free_model(Model* model) {
    if (model->vertices) {
        free(model->vertices);
        model->vertices = NULL;
    }

    if (model->vbo) {
        glDeleteBuffers(1, &model->vbo);
    }
    if (model->vao) {
        glDeleteVertexArrays(1, &model->vao);
    }
}

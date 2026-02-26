#define FAST_OBJ_IMPLEMENTATION

#include "model.h"

#include "fast_obj.h"
#include <GL/gl.h>

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
            strcpy(model_cache[model_count].filename, filename);
            Model* loaded_model = &(model_cache[model_count].model);
            model_count++;
            printf("Info: Model loaded: %s\n", filename);
            return loaded_model;
        }
    } else {
        printf("Error: Model cache is full");
    }

    return NULL;
}

void free_model_cache() {
    for (int i = 0; i < model_count; i++) {
        free_model(&(model_cache[i].model));
    }

    model_count = 0;
    printf("Info: Model cache freed\n");

    return;
}

bool load_model(Model* model, const char* filename) {
    fastObjMesh* mesh = fast_obj_read(filename);
    if (!mesh) {
        printf("Error: Failed to load obj: %s\n", filename);
        return false;
    }

    model->vertex_count = mesh->face_count * 3;
    model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));

    if (!model->vertices) {
        fast_obj_destroy(mesh);
        return false;
    }

    for (int i = 0; i < model->vertex_count; i++) {
        fastObjIndex idx = mesh->indices[i];

        model->vertices[i].position.x = mesh->positions[idx.p * 3 + 0];
        model->vertices[i].position.y = mesh->positions[idx.p * 3 + 1];
        model->vertices[i].position.z = mesh->positions[idx.p * 3 + 2];

        if (mesh->normal_count > 0) {
            model->vertices[i].normal.x = mesh->normals[idx.n * 3 + 0];
            model->vertices[i].normal.y = mesh->normals[idx.n * 3 + 1];
            model->vertices[i].normal.z = mesh->normals[idx.n * 3 + 2];
        }

        if (mesh->texcoord_count > 0) {
            model->vertices[i].u = mesh->texcoords[idx.t * 2 + 0];
            model->vertices[i].v = mesh->texcoords[idx.t * 2 + 1];
        }
    }

    fast_obj_destroy(mesh);

    return true;
}

void free_model(Model* model) {
    if (model->vertices) {
        free(model->vertices);
        model->vertices = NULL;
    }
}

void draw_model(const Model* model) {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < model->vertex_count; i++) {
        glNormal3f(model->vertices[i].normal.x, model->vertices[i].normal.y, model->vertices[i].normal.z);
        glTexCoord2f(model->vertices[i].u, model->vertices[i].v);
        glVertex3f(model->vertices[i].position.x, model->vertices[i].position.y, model->vertices[i].position.z);
    }
    glEnd();
}

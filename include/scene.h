#ifndef SCENE_H
#define SCENE_H

#include "model.h"
#include "camera.h"
#include "texture.h"

#define MAX_ENTITIES 100

typedef struct {
    Model* model;
    float x, y, z;
    float rx, ry, rz;
} Entity;

typedef struct {
    Material material;
    Entity entities[MAX_ENTITIES];
    int entity_count;
} Scene;

void init_scene(Scene* scene);

void update_scene(Scene* scene);

void render_scene(const Scene* scene);

void draw_floor();

void create_entity(Scene* scene, const char* modelname, float x, float y, float z, float rx, float ry, float rz);

#endif

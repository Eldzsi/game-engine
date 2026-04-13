#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"
#include "shader.h"
#include "collision.h"
#include "terrain.h"

#include <stdbool.h>

struct Scene; 
typedef struct Scene Scene;

typedef struct {
    vec3 position;
    vec3 rotation;
    vec3 speed;
    float vertical_velocity;
    bool is_grounded, is_crouching, is_sprinting, is_jumping;
    float current_height;
    mat4 view_matrix;
    int ground_entity_id;
} Camera;

void init_camera(Camera* camera);
void update_camera(Camera* camera, Scene* scene, Terrain* terrain, double time);
void camera_jump(Camera* camera);
void set_view_matrix(Camera* camera, Shader* shader);
void rotate_camera(Camera* camera, double horizontal, double vertical);
void set_camera_speed(Camera* camera, double speed);
void set_camera_side_speed(Camera* camera, double speed);

#endif

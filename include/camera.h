#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"

#include <stdbool.h>

typedef struct Camera {
    vec3 position;
    vec3 rotation;
    vec3 speed;

    float vertical_velocity;
    bool is_grounded;
    bool is_crouching;
    bool is_sprinting;
    
    float current_height;
} Camera;

void init_camera(Camera* camera);

void update_camera(Camera* camera, double time);

void camera_jump(Camera* camera);

void set_view(const Camera* camera);

void rotate_camera(Camera* camera, double horizontal, double vertical);

void set_camera_speed(Camera* camera, double speed);

void set_camera_side_speed(Camera* camera, double speed);

#endif

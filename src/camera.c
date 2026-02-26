#include "camera.h"

#include <GL/gl.h>

#include <math.h>

void init_camera(Camera* camera) {
    camera->position.x = 0.0;
    camera->position.y = 5.0;
    camera->position.z = 1.0; 
    camera->rotation.x = 0.0;
    camera->rotation.y = 0.0;
    camera->rotation.z = -90.0;
    camera->speed.x = 0.0;
    camera->speed.y = 0.0;
    camera->speed.z = 0.0;

    camera->vertical_velocity = 0.0f;
    camera->is_grounded = true;
    camera->is_crouching = false;
    camera->is_sprinting = false;
}

void update_camera(Camera* camera, double time) {
    float base_speed = 3.0f; 
    
    if (camera->is_sprinting) base_speed *= 2.0f; 
    if (camera->is_crouching) base_speed *= 0.5f; 

    double angle = degree_to_radian(camera->rotation.z);
    double side_angle = degree_to_radian(camera->rotation.z + 90.0);

    camera->position.x += cos(angle) * camera->speed.y * base_speed * time;
    camera->position.y += sin(angle) * camera->speed.y * base_speed * time;
    camera->position.x += cos(side_angle) * camera->speed.x * base_speed * time;
    camera->position.y += sin(side_angle) * camera->speed.x * base_speed * time;

    float gravity = -12.0f;
    float target_height = camera->is_crouching ? 0.5f : 1.0f; 

    if (camera->vertical_velocity > 0 || camera->position.z > target_height) {
        camera->is_grounded = false;
        camera->vertical_velocity += gravity * (float)time;
        camera->position.z += camera->vertical_velocity * (float)time;

        if (camera->position.z <= target_height) {
            camera->position.z = target_height;
            camera->vertical_velocity = 0;
            camera->is_grounded = true;
        }
    } 
    else if (camera->position.z < target_height) {
        camera->is_grounded = true;
        camera->position.z += 2.5f * (float)time; 
        if (camera->position.z > target_height) camera->position.z = target_height;
    } 
    else {
        camera->is_grounded = true;
        camera->vertical_velocity = 0;
    }
}

void camera_jump(Camera* camera) {
    if (camera->is_grounded) {
        camera->vertical_velocity = 5.0f;
        camera->is_grounded = false;
    }
}

void set_view(const Camera* camera) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-(camera->rotation.x + 90), 1.0, 0, 0);
    glRotatef(-(camera->rotation.y), 0, 1.0, 0);
    glRotatef(-(camera->rotation.z - 90), 0, 0, 1.0);
    glTranslatef(-camera->position.x, -camera->position.y, -camera->position.z);
}

void rotate_camera(Camera* camera, double horizontal, double vertical) {
    camera->rotation.z += horizontal;
    camera->rotation.x += vertical;

    if (camera->rotation.z < 0.0) {
        camera->rotation.z += 360.0;
    }

    if (camera->rotation.z > 360.0) {
        camera->rotation.z -= 360.0;
    }

    if (camera->rotation.x < -90.0) {
        camera->rotation.x = -90.0;
    }

    if (camera->rotation.x > 90.0) {
        camera->rotation.x = 90.0;
    }
}

void set_camera_speed(Camera* camera, double speed) {
    camera->speed.y = speed;
}

void set_camera_side_speed(Camera* camera, double speed) {
    camera->speed.x = speed;
}
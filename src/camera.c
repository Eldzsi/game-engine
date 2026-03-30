#include "camera.h"
#include "scene.h"
#include "utils.h"
#include "collision.h"

#include <glad/glad.h>
#include <math.h>

static void handle_camera_platforms(Camera* camera, Scene* scene);
static void handle_camera_movement(Camera* camera, Scene* scene, double time);
static void handle_camera_crouch(Camera* camera, Scene* scene, double time);
static void handle_camera_physics(Camera* camera, Scene* scene, double time);
static OBB get_camera_obb(Camera* camera);
static int check_camera_collision(Camera* camera, Scene* scene);
static void rotate_point_by_deltas(float drx, float dry, float drz, vec3 in_out_offset);
static void apply_gravity_and_vertical_collisions(Camera* camera, Scene* scene, double time);

void init_camera(Camera* camera) {
    camera->position.x = 0.0f;
    camera->position.y = 5.0f;
    camera->position.z = 50.0f; 
    camera->rotation.x = 0.0f;
    camera->rotation.y = 0.0f;
    camera->rotation.z = -90.0f;
    camera->speed.x = 0.0f;
    camera->speed.y = 0.0f;
    camera->speed.z = 0.0f;

    camera->vertical_velocity = 0.0f;
    camera->is_grounded = true;
    camera->is_crouching = false;
    camera->is_sprinting = false;
    camera->is_jumping = false;
    camera->current_height = 1.8f;

    camera->ground_entity_id = -1;
}

void update_camera(Camera* camera, Scene* scene, double time) {
    handle_camera_platforms(camera, scene);
    handle_camera_movement(camera, scene, time);
    handle_camera_crouch(camera, scene, time);
    handle_camera_physics(camera, scene, time);
}

void camera_jump(Camera* camera) {
    if (camera->is_grounded) {
        camera->vertical_velocity = 5.0f;
        camera->is_grounded = false;
        camera->is_jumping = true;
    }
}

void set_view(Camera* camera, Shader* shader) {
    glm_mat4_identity(camera->view_matrix);
    glm_rotate(camera->view_matrix, -glm_rad(camera->rotation.x), (float[]){1.0f, 0.0f, 0.0f});
    glm_rotate(camera->view_matrix, glm_rad(-90.0f), (float[]){1.0f, 0.0f, 0.0f});
    glm_rotate(camera->view_matrix, -glm_rad(camera->rotation.z + 90.0f), (float[]){0.0f, 0.0f, 1.0f});
    glm_translate(camera->view_matrix, (float[]){-camera->position.x, -camera->position.y, -camera->position.z});

    glUniformMatrix4fv(glGetUniformLocation(shader->id, "view"), 1, GL_FALSE, (float*)camera->view_matrix);
    glUniform3f(glGetUniformLocation(shader->id, "viewPos"), camera->position.x, camera->position.y, camera->position.z);
}

void rotate_camera(Camera* camera, double horizontal, double vertical) {
    camera->rotation.z -= (float)horizontal * 0.1f;
    camera->rotation.x -= (float)vertical * 0.1f;

    if (camera->rotation.z < 0.0) camera->rotation.z += 360.0;
    if (camera->rotation.z > 360.0) camera->rotation.z -= 360.0;
    if (camera->rotation.x < -89.0) camera->rotation.x = -89.0;
    if (camera->rotation.x > 89.0) camera->rotation.x = 89.0;
}

void set_camera_speed(Camera* camera, double speed) {
    camera->speed.y = speed;
}

void set_camera_side_speed(Camera* camera, double speed) {
    camera->speed.x = speed;
}

static OBB get_camera_obb(Camera* camera) {
    OBB obb;
    obb.extents[0] = 0.1f;
    obb.extents[1] = 0.1f;
    obb.extents[2] = (camera->current_height / 2.0f) - 0.05f; 

    obb.center[0] = camera->position.x;
    obb.center[1] = camera->position.y;
    obb.center[2] = camera->position.z - obb.extents[2] + 0.05f;

    glm_vec3_copy((vec3){1.0f, 0.0f, 0.0f}, obb.axes[0]);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, obb.axes[1]);
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, obb.axes[2]);

    return obb;
}

static int check_camera_collision(Camera* camera, Scene* scene) {
    OBB cam_box = get_camera_obb(camera);
    for (int i = 0; i < scene->entity_count; i++) {
        if (!scene->entities[i].is_active || !scene->entities[i].is_solid) continue;
        
        OBB ent_box = get_entity_obb(&(scene->entities[i]));
        if (check_obb_collision(cam_box, ent_box)) {
            return i; 
        }
    }
    
    return -1; 
}

static void rotate_point_by_deltas(float drx, float dry, float drz, vec3 in_out_offset) {
    mat4 rot = GLM_MAT4_IDENTITY_INIT;
    glm_rotate(rot, glm_rad(drx), (float[]){1.0f, 0.0f, 0.0f});
    glm_rotate(rot, glm_rad(dry), (float[]){0.0f, 1.0f, 0.0f});
    glm_rotate(rot, glm_rad(drz), (float[]){0.0f, 0.0f, 1.0f});

    vec3 rotated_offset;
    glm_mat4_mulv3(rot, in_out_offset, 1.0f, rotated_offset);
    
    in_out_offset[0] = rotated_offset[0];
    in_out_offset[1] = rotated_offset[1];
    in_out_offset[2] = rotated_offset[2];
}

static void handle_camera_platforms(Camera* camera, Scene* scene) {
    if (camera->is_grounded && camera->ground_entity_id != -1) {
        Entity* ground = &(scene->entities[camera->ground_entity_id]);
        if (ground->is_active && (ground->delta_x != 0.0f || ground->delta_y != 0.0f || ground->delta_z != 0.0f || 
                                  ground->delta_rx != 0.0f || ground->delta_ry != 0.0f || ground->delta_rz != 0.0f)) {
            
            float old_cx = ground->x - ground->delta_x;
            float old_cy = ground->y - ground->delta_y;
            float old_cz = ground->z - ground->delta_z;

            vec3 offset = {camera->position.x - old_cx, camera->position.y - old_cy, camera->position.z - old_cz};
            rotate_point_by_deltas(ground->delta_rx, ground->delta_ry, ground->delta_rz, offset);

            camera->position.x = ground->x + offset[0];
            camera->position.y = ground->y + offset[1];
            camera->position.z = ground->z + offset[2];
        }
    }

    OBB cam_box = get_camera_obb(camera); 
    for (int i = 0; i < scene->entity_count; i++) {
        Entity* e = &(scene->entities[i]);
        
        if (e->is_active && e->is_solid && camera->ground_entity_id != i) {
            if (e->delta_x != 0.0f || e->delta_y != 0.0f || e->delta_z != 0.0f || 
                e->delta_rx != 0.0f || e->delta_ry != 0.0f || e->delta_rz != 0.0f) {
                
                if (check_obb_collision(cam_box, get_entity_obb(e))) {
                    float old_cx = e->x - e->delta_x;
                    float old_cy = e->y - e->delta_y;
                    float old_cz = e->z - e->delta_z;

                    vec3 offset = {camera->position.x - old_cx, camera->position.y - old_cy, camera->position.z - old_cz};
                    vec3 original_offset = {offset[0], offset[1], offset[2]};
                    
                    rotate_point_by_deltas(e->delta_rx, e->delta_ry, e->delta_rz, offset);

                    float push_x = e->delta_x + (offset[0] - original_offset[0]);
                    float push_y = e->delta_y + (offset[1] - original_offset[1]);

                    camera->position.x += push_x;
                    camera->position.y += push_y;
                    
                    cam_box = get_camera_obb(camera); 
                }
            }
        }
    }
}

static void handle_camera_movement(Camera* camera, Scene* scene, double time) {
    float base_speed = 3.0f;
    if (camera->is_sprinting) base_speed *= 2.0f;
    if (camera->is_crouching) base_speed *= 0.5f;

    double angle = degree_to_radian(camera->rotation.z);
    double side_angle = angle + degree_to_radian(90.0);

    float dx = (-(float)cos(angle) * (float)camera->speed.y + (float)cos(side_angle) * (float)camera->speed.x) * base_speed * (float)time;
    float dy = (-(float)sin(angle) * (float)camera->speed.y + (float)sin(side_angle) * (float)camera->speed.x) * base_speed * (float)time;

    float step_offset = 0.4f;

    camera->position.x += dx;
    if (check_camera_collision(camera, scene) != -1) {
        float old_z = camera->position.z;
        camera->position.x -= dx; 
        if (camera->is_grounded) {
            camera->position.z += step_offset;
            if (check_camera_collision(camera, scene) == -1) {
                camera->position.x += dx;
                if (check_camera_collision(camera, scene) != -1) {
                    camera->position.x -= dx;
                    camera->position.z = old_z;
                }
            } else {
                camera->position.z = old_z;
            }
        }
    }

    camera->position.y += dy;
    if (check_camera_collision(camera, scene) != -1) {
        float old_z = camera->position.z;
        camera->position.y -= dy;
        if (camera->is_grounded) {
            camera->position.z += step_offset;
            if (check_camera_collision(camera, scene) == -1) {
                camera->position.y += dy;
                if (check_camera_collision(camera, scene) != -1) {
                    camera->position.y -= dy;
                    camera->position.z = old_z;
                }
            } else {
                camera->position.z = old_z;
            }
        }
    }
}

static void handle_camera_crouch(Camera* camera, Scene* scene, double time) {
    float target_height = camera->is_crouching ? 1.0f : 1.8f;
    float diff = target_height - camera->current_height;

    if (fabs(diff) > 0.001f) {
        float step = diff * 10.0f * (float)time;
        camera->current_height += step;
        camera->position.z += step;

        if (check_camera_collision(camera, scene) != -1) { 
            camera->current_height -= step;
            camera->position.z -= step;
        }
    }
}

static void apply_gravity_and_vertical_collisions(Camera* camera, Scene* scene, double time) {
    float gravity = -12.0f;
    float old_z = camera->position.z;
    camera->vertical_velocity += gravity * (float)time;
    camera->position.z += camera->vertical_velocity * (float)time;

    if (check_camera_collision(camera, scene) != -1) {
        camera->position.z = old_z;
        if (camera->vertical_velocity > 0.0f) {
            camera->vertical_velocity = 0.0f; 
        } else {
            camera->vertical_velocity *= 0.5f; 
        }
    }
}

static void handle_camera_physics(Camera* camera, Scene* scene, double time) {
    apply_gravity_and_vertical_collisions(camera, scene, time);

    float ground_z = -1e10f; 
    int hit_id = -1;
    float foot_z_for_check = camera->position.z - camera->current_height;

    float offsets_x[] = {0.0f, 0.1f, -0.1f, 0.1f, -0.1f};
    float offsets_y[] = {0.0f, 0.1f, 0.1f, -0.1f, -0.1f};

    for (int i = 0; i < scene->entity_count; i++) {
        if (!scene->entities[i].is_active || !scene->entities[i].is_solid) continue;
        
        OBB ent_box = get_entity_obb(&(scene->entities[i]));
        
        for(int r = 0; r < 5; r++) {
            float ray_x = camera->position.x + offsets_x[r];
            float ray_y = camera->position.y + offsets_y[r];
            
            vec3 hit_normal;
            float hit_z = get_vertical_ray_obb_collision(ent_box, ray_x, ray_y, camera->position.z, hit_normal);

            if (hit_z > -1e9f) {
                if (hit_normal[2] < 0.7f) {
                    if (hit_z >= foot_z_for_check && hit_z <= camera->position.z + 0.5f) {
                        float old_x = camera->position.x;
                        float old_y = camera->position.y;
                        
                        camera->position.x += hit_normal[0] * 3.0f * (float)time;
                        camera->position.y += hit_normal[1] * 3.0f * (float)time;
                        
                        if (check_camera_collision(camera, scene) != -1) {
                            camera->position.x = old_x;
                            camera->position.y = old_y;
                        }
                    }
                    continue; 
                }

                if (hit_z <= foot_z_for_check + 0.5f && hit_z > ground_z) {
                    ground_z = hit_z;
                    hit_id = i;
                }
            }
        }
    }

    float foot_z = camera->position.z - camera->current_height;
    bool stick_to_ground = (camera->is_grounded && !camera->is_jumping && (foot_z - ground_z) <= 0.4f);
    
    if (foot_z <= ground_z || stick_to_ground) {
        camera->position.z = ground_z + camera->current_height;
        camera->vertical_velocity = 0.0f;
        camera->is_grounded = true;
        camera->is_jumping = false;
        camera->ground_entity_id = hit_id;
    } else {
        camera->is_grounded = false;
    }
}

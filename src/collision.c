#include "collision.h"
#include "scene.h"

#include <math.h>

bool check_aabb_collision(AABB a, AABB b) {
    return (a.min_x <= b.max_x && a.max_x >= b.min_x) && 
           (a.min_y <= b.max_y && a.max_y >= b.min_y) && 
           (a.min_z <= b.max_z && a.max_z >= b.min_z);
}

float get_vertical_ray_collision(AABB box, float x, float y, float current_z) {
    if (x >= box.min_x && x <= box.max_x && y >= box.min_y && y <= box.max_y) {
        if (box.max_z <= current_z + 0.5f) { 
            return box.max_z;
        }
    }
    return -1e10f;
}

static bool test_axis(vec3 axis, vec3 t, OBB a, OBB b) {
    if (glm_vec3_norm2(axis) < 0.0001f) return false;

    vec3 n;
    glm_vec3_normalize_to(axis, n);

    float rA = a.extents[0] * fabs(glm_vec3_dot(a.axes[0], n)) +
               a.extents[1] * fabs(glm_vec3_dot(a.axes[1], n)) +
               a.extents[2] * fabs(glm_vec3_dot(a.axes[2], n));

    float rB = b.extents[0] * fabs(glm_vec3_dot(b.axes[0], n)) +
               b.extents[1] * fabs(glm_vec3_dot(b.axes[1], n)) +
               b.extents[2] * fabs(glm_vec3_dot(b.axes[2], n));

    float distance = fabs(glm_vec3_dot(t, n));
    return distance > (rA + rB); 
}

bool check_obb_collision(OBB a, OBB b) {
    vec3 t;
    glm_vec3_sub(b.center, a.center, t);

    for (int i = 0; i < 3; i++) {
        if (test_axis(a.axes[i], t, a, b)) return false;
        if (test_axis(b.axes[i], t, a, b)) return false;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vec3 cross_axis;
            glm_vec3_cross(a.axes[i], b.axes[j], cross_axis);
            if (test_axis(cross_axis, t, a, b)) return false;
        }
    }
    return true; 
}

float get_vertical_ray_obb_collision(OBB box, float x, float y, float current_z, vec3 out_normal) {
    vec3 diff = {x - box.center[0], y - box.center[1], current_z - box.center[2]};
    vec3 ray_d = {0.0f, 0.0f, -1.0f};

    vec3 local_o, local_d;
    for(int i=0; i<3; i++) {
        local_o[i] = glm_vec3_dot(diff, box.axes[i]);
        local_d[i] = glm_vec3_dot(ray_d, box.axes[i]);
    }

    float tmin = -1e10f, tmax = 1e10f;
    int hit_axis = -1;

    for (int i = 0; i < 3; i++) {
        if (fabs(local_d[i]) < 0.00001f) {
            if (local_o[i] < -box.extents[i] || local_o[i] > box.extents[i]) return -1e10f;
        } else {
            float ood = 1.0f / local_d[i];
            float t1 = (-box.extents[i] - local_o[i]) * ood;
            float t2 = (box.extents[i] - local_o[i]) * ood;
            if (t1 > t2) { float temp = t1; t1 = t2; t2 = temp; }
            if (t1 > tmin) {
                tmin = t1;
                hit_axis = i;
            }
            if (t2 < tmax) tmax = t2;
        }
    }

    if (tmax > 0.0f && tmin < tmax) {
        if (tmin < 0.0f) tmin = 0.0f;
        if (out_normal && hit_axis != -1) {
            glm_vec3_copy(box.axes[hit_axis], out_normal);
            if (out_normal[2] < 0) glm_vec3_negate(out_normal);
        }
        return current_z - tmin;
    }
    return -1e10f;
}

bool ray_intersects_obb(vec3 ray_origin, vec3 ray_dir, OBB box, float* out_dist) {
    float tMin = 0.0f;
    float tMax = 100000.0f;
    vec3 delta;
    glm_vec3_sub(box.center, ray_origin, delta);

    for (int i = 0; i < 3; i++) {
        vec3 axis;
        glm_vec3_copy(box.axes[i], axis);
        
        float e = glm_vec3_dot(axis, delta);
        float f = glm_vec3_dot(ray_dir, axis);

        if (fabs(f) > 0.001f) {
            float t1 = (e + box.extents[i]) / f;
            float t2 = (e - box.extents[i]) / f;
            
            if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }
            if (t2 < tMax) tMax = t2;
            if (t1 > tMin) tMin = t1;
            if (tMax < tMin) return false;
        } else {
            if (-e - box.extents[i] > 0.0f || -e + box.extents[i] < 0.0f) return false;
        }
    }
    
    if (out_dist) *out_dist = tMin;
    return true;
}

OBB get_entity_obb(const Entity* entity) {
    OBB obb;
    Model* m_data = entity->model;

    obb.extents[0] = (m_data->base_extents[0] * entity->sx) / 2.0f;
    obb.extents[1] = (m_data->base_extents[1] * entity->sy) / 2.0f;
    obb.extents[2] = (m_data->base_extents[2] * entity->sz) / 2.0f;

    mat4 rot = GLM_MAT4_IDENTITY_INIT;
    glm_rotate(rot, glm_rad(entity->rx), (float[]){1.0f, 0.0f, 0.0f});
    glm_rotate(rot, glm_rad(entity->ry), (float[]){0.0f, 1.0f, 0.0f});
    glm_rotate(rot, glm_rad(entity->rz), (float[]){0.0f, 0.0f, 1.0f});

    vec3 offset_scaled = {
        m_data->base_offset[0] * entity->sx,
        m_data->base_offset[1] * entity->sy,
        m_data->base_offset[2] * entity->sz
    };
    
    vec3 rotated_offset;
    glm_mat4_mulv3(rot, offset_scaled, 1.0f, rotated_offset);

    obb.center[0] = entity->x + rotated_offset[0];
    obb.center[1] = entity->y + rotated_offset[1];
    obb.center[2] = entity->z + rotated_offset[2];

    for(int i = 0; i < 3; i++) {
        obb.axes[0][i] = rot[0][i];
        obb.axes[1][i] = rot[1][i];
        obb.axes[2][i] = rot[2][i];
    }

    return obb;
}

bool raycast_scene(Scene* scene, vec3 start, vec3 end, vec3 hit_point, int* hit_entity_id) {
    vec3 ray_dir;
    glm_vec3_sub(end, start, ray_dir);
    
    float max_distance = glm_vec3_norm(ray_dir);
    glm_vec3_normalize(ray_dir);

    bool found_hit = false;
    float closest_hit_distance = max_distance;

    for (int i = 0; i < scene->entity_count; ++i) {
        Entity* e = &(scene->entities[i]);
        
        if (!e->is_active || !e->is_solid) continue;

        OBB box = get_entity_obb(e);
        float hit_dist;

        if (ray_intersects_obb(start, ray_dir, box, &hit_dist)) {
            if (hit_dist < closest_hit_distance && hit_dist < max_distance) {
                closest_hit_distance = hit_dist;
                *hit_entity_id = i;
                found_hit = true;
            }
        }
    }

    if (found_hit) {
        hit_point[0] = start[0] + ray_dir[0] * closest_hit_distance;
        hit_point[1] = start[1] + ray_dir[1] * closest_hit_distance;
        hit_point[2] = start[2] + ray_dir[2] * closest_hit_distance;
    }

    return found_hit;
}

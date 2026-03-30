#ifndef COLLISION_H
#define COLLISION_H

#include "utils.h"

#include <cglm/cglm.h>
#include <stdbool.h>

struct Scene;
struct Entity;

typedef struct {
    float min_x, max_x;
    float min_y, max_y;
    float min_z, max_z;
} AABB;

typedef struct {
    vec3 center;
    vec3 axes[3];
    float extents[3];
} OBB;

bool check_aabb_collision(AABB a, AABB b);
bool check_obb_collision(OBB a, OBB b);
float get_vertical_ray_collision(AABB box, float x, float y, float current_z);
float get_vertical_ray_obb_collision(OBB box, float x, float y, float current_z, vec3 out_normal);
bool ray_intersects_obb(vec3 ray_origin, vec3 ray_dir, OBB box, float* out_dist);
OBB get_entity_obb(const struct Entity* entity);
bool raycast_scene(struct Scene* scene, vec3 start, vec3 end, vec3 hit_point, int* hit_entity_id);

#endif

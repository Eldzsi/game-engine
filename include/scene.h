#ifndef SCENE_H
#define SCENE_H

#include "model.h"
#include "camera.h"
#include "texture.h"
#include "utils.h"
#include "shader.h"
#include "terrain.h"

#define MAX_ENTITIES 2000
#define MAX_POINT_LIGHTS 20
#define MAX_SPOT_LIGHTS 20

#define MAX_MATERIALS 8

typedef struct Entity {
    Model* model;
    
    GLuint textures[MAX_MATERIALS];
    int texture_count;

    float x, y, z;
    float rx, ry, rz;
    float sx, sy, sz;
    float cx, cy, cz;

    float spec_r;
    float spec_g;
    float spec_b;
    float shininess;
    
    bool is_active;
    bool is_moving;
    bool is_visible;
    bool is_solid;

    bool has_glow;
    float glow_r;
    float glow_g;
    float glow_b;
    
    float target_x, target_y, target_z;
    float target_rx, target_ry, target_rz;
    float start_x, start_y, start_z;
    float start_rx, start_ry, start_rz;
    float move_time;
    float elapsed_move_time;
    float delta_x, delta_y, delta_z;
    float delta_rx, delta_ry, delta_rz;
    float prev_x, prev_y, prev_z;
    float prev_rx, prev_ry, prev_rz;

    float uv_speed_x;
    float uv_speed_y;

    bool attached_to_camera;
    float attach_ox, attach_oy, attach_oz;
    float attach_rx, attach_ry, attach_rz;
} Entity;

typedef struct PointLight {
    float x, y, z;
    float r, g, b;
    float constant, linear, quadratic;
    bool is_active;
} PointLight;

typedef struct AmbientLight {
    float r, g, b;
} AmbientLight;

typedef struct SpotLight {
    float x, y, z;
    float dir_x, dir_y, dir_z;
    float r, g, b;
    float cutOff, outerCutOff;
    float constant, linear, quadratic;
    bool is_active; 
} SpotLight;

typedef struct DirectionalLight {
    float dir_x, dir_y, dir_z;
    float r, g, b;
} DirectionalLight;

typedef struct {
    GLuint vao, vbo;
    GLuint texture_id;
    Shader shader;
    bool is_active;
} Skybox;

typedef struct Scene {
    Material material;
    Entity entities[MAX_ENTITIES];
    int entity_count;
    
    AmbientLight ambient;
    DirectionalLight directional_light;
    
    PointLight point_lights[MAX_POINT_LIGHTS];
    int point_light_count;

    SpotLight spot_lights[MAX_SPOT_LIGHTS];
    int spot_light_count;

    float sky_r, sky_g, sky_b;
    Skybox skybox;

    Terrain terrain;
} Scene;

bool get_show_colliders();
void set_show_colliders(bool show);
void init_scene(Scene* scene);
void render_scene(const Scene* scene, Camera* camera, Shader* shader);
void update_scene(Scene* scene, double elapsed_time);
int create_entity(Scene* scene, const char* modelname, const char** texturenames, int texture_count, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz);
void set_entity_scale(Scene* scene, int entity_id, float sx, float sy, float sz);
void set_entity_collider_scale(Scene* scene, int entity_id, float cx, float cy, float cz);
void set_entity_glow(Scene* scene, int entity_id, bool has_glow, float r, float g, float b);
void set_entity_material(Scene* scene, int entity_id, float spec_r, float spec_g, float spec_b, float shininess);
void set_entity_uv_speed(Scene* scene, int entity_id, float speed_x, float speed_y);
void attach_entity_to_camera(Scene* scene, int entity_id, float ox, float oy, float oz, float rx, float ry, float rz);
void set_ambient_light(Scene* scene, float r, float g, float b);
void set_directional_light(Scene* scene, float dx, float dy, float dz, float r, float g, float b);
int add_point_light(Scene* scene, float x, float y, float z, float r, float g, float b, float constant, float linear, float quadratic);
int add_spot_light(Scene* scene, float x, float y, float z, float dx, float dy, float dz, float r, float g, float b, float cutOff, float outerCutOff, float constant, float linear, float quadratic);
void set_point_light(Scene* scene, int id, float x, float y, float z, float r, float g, float b);
void set_spot_light(Scene* scene, int id, float x, float y, float z, float dx, float dy, float dz, float r, float g, float b);
void init_skybox(Scene* scene);
void render_skybox(Scene* scene, const Camera* camera, mat4 projection_matrix);
void destroy_skybox(Scene* scene);

#endif

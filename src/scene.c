#include "scene.h"
#include "model.h"
#include "texture.h"
#include "collision.h"

#include <cglm/cglm.h>
#include <glad/glad.h>

#include <stdio.h>
#include <math.h>

static bool global_show_colliders = false;

static void update_entity(Entity* e, float elapsed_time);
static void bind_lights_to_shader(const Scene* scene, Shader* shader);
static void render_debug_collider(const Entity* e, Shader* shader);

void set_show_colliders(bool show) {
    global_show_colliders = show;
}

bool get_show_colliders() {
    return global_show_colliders;
}

void init_scene(Scene* scene) {
    scene->entity_count = 0;

    scene->material.specular.red = 0.3;
    scene->material.specular.green = 0.3;
    scene->material.specular.blue = 0.3;

    scene->material.shininess = 50.0;

    scene->ambient.r = 0.0f; 
    scene->ambient.g = 0.0f; 
    scene->ambient.b = 0.0f;

    scene->sun.dir_x = 0.0f;
    scene->sun.dir_y = -1.0f;
    scene->sun.dir_z = -0.5f;
    scene->sun.r = 0.0f; 
    scene->sun.g = 0.0f; 
    scene->sun.b = 0.0f;

    scene->point_light_count = 0;
    scene->spot_light_count = 0;

    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        scene->point_lights[i].is_active = false;
    }
    for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {
        scene->spot_lights[i].is_active = false;
    }

    scene->sky_r = 1.0f;
    scene->sky_g = 1.0f;
    scene->sky_b = 1.0f;
}

int create_entity(Scene* scene, const char* modelname, const char* texturename, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) {
    int free_slot = -1;
    
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!scene->entities[i].is_active) {
            free_slot = i;
            break;
        }
    }

    if (free_slot == -1) {
        printf("Error: Max entities reached\n");
        return -1;
    }

    if (free_slot >= scene->entity_count) {
        scene->entity_count = free_slot + 1;
    }

    Model* model = get_model(modelname);
    GLuint tex = get_texture(texturename);

    if (model != NULL) {
        Entity* e = &(scene->entities[free_slot]);
        e->model = model;
        e->texture = tex; 
        e->x = x; e->y = y; e->z = z;
        e->rx = rx; e->ry = ry; e->rz = rz;
        e->sx = sx; e->sy = sy; e->sz = sz;

        e->spec_r = 0.3f;
        e->spec_g = 0.3f;
        e->spec_b = 0.3f;
        e->shininess = 50.0f;

        e->prev_x = x; e->prev_y = y; e->prev_z = z;
        e->prev_rx = rx; e->prev_ry = ry; e->prev_rz = rz;
        
        e->is_active = true;
        e->is_moving = false;
        e->is_visible = true;
        e->is_solid = true;

        e->has_glow = false;
        e->glow_r = 1.0f; 
        e->glow_g = 1.0f; 
        e->glow_b = 1.0f;
        
        return free_slot;
    }

    return -1;
}

void set_entity_scale(Scene* scene, int entity_id, float sx, float sy, float sz) {
    if (entity_id >= 0 && entity_id < scene->entity_count) {
        scene->entities[entity_id].sx = sx;
        scene->entities[entity_id].sy = sy;
        scene->entities[entity_id].sz = sz;
    } else {
        printf("Warning: Tried to scale non-existent entity ID: %d\n", entity_id);
    }
}

void set_entity_glow(Scene* scene, int entity_id, bool has_glow, float r, float g, float b) {
    if (entity_id >= 0 && entity_id < scene->entity_count) {
        scene->entities[entity_id].has_glow = has_glow;
        scene->entities[entity_id].glow_r = r;
        scene->entities[entity_id].glow_g = g;
        scene->entities[entity_id].glow_b = b;
    }
}

void set_entity_material(Scene* scene, int entity_id, float spec_r, float spec_g, float spec_b, float shininess) {
    if (entity_id >= 0 && entity_id < scene->entity_count) {
        scene->entities[entity_id].spec_r = spec_r;
        scene->entities[entity_id].spec_g = spec_g;
        scene->entities[entity_id].spec_b = spec_b;
        scene->entities[entity_id].shininess = shininess;
    }
}

void render_scene(const Scene* scene, Shader* shader) {
    bind_lights_to_shader(scene, shader);

    for (int i = 0; i < scene->entity_count; i++) {
        const Entity* e = &(scene->entities[i]);

        if (!e->is_active) continue;
        if (!e->is_visible && !global_show_colliders) continue;
        
        mat4 model = GLM_MAT4_IDENTITY_INIT;

        glm_translate(model, (float[]){e->x, e->y, e->z});

        glm_rotate(model, glm_rad(e->rx), (float[]){1.0f, 0.0f, 0.0f});
        glm_rotate(model, glm_rad(e->ry), (float[]){0.0f, 1.0f, 0.0f});
        glm_rotate(model, glm_rad(e->rz), (float[]){0.0f, 0.0f, 1.0f});

        glm_scale(model, (float[]){e->sx, e->sy, e->sz});
        
        glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, (float*)model);
        glUniform3f(glGetUniformLocation(shader->id, "objScale"), e->sx, e->sy, e->sz);

        glUniform1i(glGetUniformLocation(shader->id, "u_has_glow"), e->has_glow);
        glUniform3f(glGetUniformLocation(shader->id, "u_glow_color"), e->glow_r, e->glow_g, e->glow_b);

        glUniform3f(glGetUniformLocation(shader->id, "specularColor"), e->spec_r, e->spec_g, e->spec_b);
        glUniform1f(glGetUniformLocation(shader->id, "shininess"), e->shininess);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, e->texture);
        glUniform1i(glGetUniformLocation(shader->id, "diffuse_texture"), 0);

        if (e->is_visible) {
            draw_model(e->model);
        }

        if (global_show_colliders && e->is_solid) {
            render_debug_collider(e, shader);
        }
    }
}

void update_scene(Scene* scene, double elapsed_time) {
    for (int i = 0; i < scene->entity_count; i++) {
        Entity* e = &(scene->entities[i]);
        if (!e->is_active) continue;
        
        update_entity(e, (float)elapsed_time);
    }
}

void set_ambient_light(Scene* scene, float r, float g, float b) {
    scene->ambient.r = r; scene->ambient.g = g; scene->ambient.b = b;
}

void set_sun_light(Scene* scene, float dx, float dy, float dz, float r, float g, float b) {
    scene->sun.dir_x = dx; scene->sun.dir_y = dy; scene->sun.dir_z = dz;
    scene->sun.r = r; scene->sun.g = g; scene->sun.b = b;
}

int add_point_light(Scene* scene, float x, float y, float z, float r, float g, float b, float constant, float linear, float quadratic) {
    int free_slot = -1;
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        if (!scene->point_lights[i].is_active) {
            free_slot = i;
            break;
        }
    }
    
    if (free_slot != -1) {
        PointLight* pl = &(scene->point_lights[free_slot]);
        pl->x = x; pl->y = y; pl->z = z;
        pl->r = r; pl->g = g; pl->b = b;
        pl->constant = constant; pl->linear = linear; pl->quadratic = quadratic;
        pl->is_active = true; 
        
        if (free_slot >= scene->point_light_count) {
            scene->point_light_count = free_slot + 1;
        }
        
        return free_slot;
    }
    
    printf("WARNING: Max point lights reached!\n");
    return -1;
}

void set_point_light(Scene* scene, int id, float x, float y, float z, float r, float g, float b) {
    if (id >= 0 && id < MAX_POINT_LIGHTS) {
        PointLight* pl = &(scene->point_lights[id]);
        if (pl->is_active) {
            pl->x = x; pl->y = y; pl->z = z;
            pl->r = r; pl->g = g; pl->b = b;
        }
    }
}

int add_spot_light(Scene* scene, float x, float y, float z, float dx, float dy, float dz, float r, float g, float b, float cutOff, float outerCutOff, float constant, float linear, float quadratic) {
    int free_slot = -1;
    for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {
        if (!scene->spot_lights[i].is_active) {
            free_slot = i;
            break;
        }
    }
    
    if (free_slot != -1) {
        SpotLight* sl = &(scene->spot_lights[free_slot]);
        sl->x = x; sl->y = y; sl->z = z;
        sl->dir_x = dx; sl->dir_y = dy; sl->dir_z = dz;
        sl->r = r; sl->g = g; sl->b = b;
        
        sl->cutOff = cosf(cutOff * (M_PI / 180.0f));
        sl->outerCutOff = cosf(outerCutOff * (M_PI / 180.0f));
        
        sl->constant = constant; sl->linear = linear; sl->quadratic = quadratic;
        sl->is_active = true;
        
        if (free_slot >= scene->spot_light_count) {
            scene->spot_light_count = free_slot + 1;
        }
        
        return free_slot;
    } 
    
    printf("WARNING: Max spot lights reached!\n");
    return -1;
}

void set_spot_light(Scene* scene, int id, float x, float y, float z, float dx, float dy, float dz, float r, float g, float b) {
    if (id >= 0 && id < MAX_SPOT_LIGHTS) {
        SpotLight* sl = &(scene->spot_lights[id]);
        if (sl->is_active) {
            sl->x = x; sl->y = y; sl->z = z;
            sl->dir_x = dx; sl->dir_y = dy; sl->dir_z = dz;
            sl->r = r; sl->g = g; sl->b = b;
        }
    }
}

static void update_entity(Entity* e, float elapsed_time) {
    if (e->is_moving) {
        e->elapsed_move_time += elapsed_time;
        float progress = e->elapsed_move_time / e->move_time;
        
        if (progress >= 1.0f) {
            progress = 1.0f;
            e->is_moving = false;
        }
        
        e->x = e->start_x + (e->target_x - e->start_x) * progress;
        e->y = e->start_y + (e->target_y - e->start_y) * progress;
        e->z = e->start_z + (e->target_z - e->start_z) * progress;
        
        e->rx = e->start_rx + (e->target_rx - e->start_rx) * progress;
        e->ry = e->start_ry + (e->target_ry - e->start_ry) * progress;
        e->rz = e->start_rz + (e->target_rz - e->start_rz) * progress;
    }

    e->delta_x = e->x - e->prev_x;
    e->delta_y = e->y - e->prev_y;
    e->delta_z = e->z - e->prev_z;

    e->delta_rx = e->rx - e->prev_rx;
    e->delta_ry = e->ry - e->prev_ry;
    e->delta_rz = e->rz - e->prev_rz;

    e->prev_x = e->x;
    e->prev_y = e->y;
    e->prev_z = e->z;
    
    e->prev_rx = e->rx;
    e->prev_ry = e->ry;
    e->prev_rz = e->rz;
}

static void bind_lights_to_shader(const Scene* scene, Shader* shader) {
    glUniform3f(glGetUniformLocation(shader->id, "ambientColor"), scene->ambient.r, scene->ambient.g, scene->ambient.b);
    glUniform3f(glGetUniformLocation(shader->id, "sunLight.direction"), scene->sun.dir_x, scene->sun.dir_y, scene->sun.dir_z);
    glUniform3f(glGetUniformLocation(shader->id, "sunLight.color"), scene->sun.r, scene->sun.g, scene->sun.b);
    
    char uniform_name[64];
    
    int active_points = 0;
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        const PointLight* pl = &(scene->point_lights[i]);
        if (!pl->is_active) continue;
        
        sprintf(uniform_name, "pointLights[%d].position", active_points);
        glUniform3f(glGetUniformLocation(shader->id, uniform_name), pl->x, pl->y, pl->z);
        
        sprintf(uniform_name, "pointLights[%d].color", active_points);
        glUniform3f(glGetUniformLocation(shader->id, uniform_name), pl->r, pl->g, pl->b);
        
        sprintf(uniform_name, "pointLights[%d].constant", active_points);
        glUniform1f(glGetUniformLocation(shader->id, uniform_name), pl->constant);
        
        sprintf(uniform_name, "pointLights[%d].linear", active_points);
        glUniform1f(glGetUniformLocation(shader->id, uniform_name), pl->linear);
        
        sprintf(uniform_name, "pointLights[%d].quadratic", active_points);
        glUniform1f(glGetUniformLocation(shader->id, uniform_name), pl->quadratic);
        
        active_points++;
    }
    glUniform1i(glGetUniformLocation(shader->id, "pointLightCount"), active_points);

    int active_spots = 0;
    for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {
        const SpotLight* sl = &(scene->spot_lights[i]);
        if (!sl->is_active) continue;
        
        sprintf(uniform_name, "spotLights[%d].position", active_spots);
        glUniform3f(glGetUniformLocation(shader->id, uniform_name), sl->x, sl->y, sl->z);
        
        sprintf(uniform_name, "spotLights[%d].direction", active_spots);
        glUniform3f(glGetUniformLocation(shader->id, uniform_name), sl->dir_x, sl->dir_y, sl->dir_z);
        
        sprintf(uniform_name, "spotLights[%d].color", active_spots);
        glUniform3f(glGetUniformLocation(shader->id, uniform_name), sl->r, sl->g, sl->b);
        
        sprintf(uniform_name, "spotLights[%d].cutOff", active_spots);
        glUniform1f(glGetUniformLocation(shader->id, uniform_name), sl->cutOff);
        
        sprintf(uniform_name, "spotLights[%d].outerCutOff", active_spots);
        glUniform1f(glGetUniformLocation(shader->id, uniform_name), sl->outerCutOff);
        
        sprintf(uniform_name, "spotLights[%d].constant", active_spots);
        glUniform1f(glGetUniformLocation(shader->id, uniform_name), sl->constant);
        
        sprintf(uniform_name, "spotLights[%d].linear", active_spots);
        glUniform1f(glGetUniformLocation(shader->id, uniform_name), sl->linear);
        
        sprintf(uniform_name, "spotLights[%d].quadratic", active_spots);
        glUniform1f(glGetUniformLocation(shader->id, uniform_name), sl->quadratic);
        
        active_spots++;
    }
    glUniform1i(glGetUniformLocation(shader->id, "spotLightCount"), active_spots);
}

static void render_debug_collider(const Entity* e, Shader* shader) {
    Model* obb_cube = get_model("assets/models/cube.obj");
    if (obb_cube == NULL) return;

    OBB actual_obb = get_entity_obb(e);

    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glUniform1i(glGetUniformLocation(shader->id, "u_has_glow"), 1);
    glUniform3f(glGetUniformLocation(shader->id, "u_glow_color"), 2.0f, 0.5f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, 0); 

    mat4 debug_mat = GLM_MAT4_IDENTITY_INIT;
    glm_translate(debug_mat, actual_obb.center);
    
    debug_mat[0][0] = actual_obb.axes[0][0]; debug_mat[0][1] = actual_obb.axes[0][1]; debug_mat[0][2] = actual_obb.axes[0][2];
    debug_mat[1][0] = actual_obb.axes[1][0]; debug_mat[1][1] = actual_obb.axes[1][1]; debug_mat[1][2] = actual_obb.axes[1][2];
    debug_mat[2][0] = actual_obb.axes[2][0]; debug_mat[2][1] = actual_obb.axes[2][1]; debug_mat[2][2] = actual_obb.axes[2][2];
    
    glm_scale(debug_mat, (float[]){
        actual_obb.extents[0] * 2.01f, 
        actual_obb.extents[1] * 2.01f, 
        actual_obb.extents[2] * 2.01f
    });
    
    glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, (float*)debug_mat);
    draw_model(obb_cube);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glUniform1i(glGetUniformLocation(shader->id, "u_has_glow"), e->has_glow);
}

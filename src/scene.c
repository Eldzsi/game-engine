#include "scene.h"
#include "camera.h"
#include "model.h"
#include "texture.h"
#include "collision.h"

#include <cglm/cglm.h>
#include <glad/glad.h>

#include <stdio.h>
#include <math.h>

static bool show_colliders = false;

static const float unit_cube_vertices[] = {
    -1.0f,  1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f
};

static void update_entity(Entity* e, float elapsed_time);
static void bind_lights_to_shader(const Scene* scene, Shader* shader);
static void render_debug_collider(const Entity* e, Shader* shader);

void set_show_colliders(bool show) {
    show_colliders = show;
}

bool get_show_colliders() {
    return show_colliders;
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

    scene->directional_light.dir_x = 0.0f;
    scene->directional_light.dir_y = -1.0f;
    scene->directional_light.dir_z = -0.5f;
    scene->directional_light.r = 0.0f; 
    scene->directional_light.g = 0.0f; 
    scene->directional_light.b = 0.0f;

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

    scene->skybox.is_active = false;
}

int create_entity(Scene* scene, const char* modelname, const char** texturenames, int texture_count, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) { 
    int free_slot = -1;

    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!scene->entities[i].is_active) {
            free_slot = i;
            break;
        }
    }

    if (free_slot == -1) {
        printf("ERROR: Max entities reached.\n");
        return -1;
    }

    if (free_slot >= scene->entity_count) {
        scene->entity_count = free_slot + 1;
    }

    Model* model = get_model(modelname);

    if (model != NULL) {
        Entity* e = &(scene->entities[free_slot]);
        e->model = model;
        
        e->texture_count = texture_count > MAX_MATERIALS ? MAX_MATERIALS : texture_count;
        for (int i = 0; i < e->texture_count; i++) {
            e->textures[i] = get_texture(texturenames[i]);
        }

        e->x = x;
        e->y = y;
        e->z = z;
        e->rx = rx;
        e->ry = ry;
        e->rz = rz;
        e->sx = sx;
        e->sy = sy;
        e->sz = sz;

        e->cx = sx;
        e->cy = sy;
        e->cz = sz;

        e->spec_r = 0.3f;
        e->spec_g = 0.3f;
        e->spec_b = 0.3f;
        e->shininess = 50.0f;

        e->prev_x = x;
        e->prev_y = y;
        e->prev_z = z;
        e->prev_rx = rx;
        e->prev_ry = ry;
        e->prev_rz = rz;
        
        e->is_active = true;
        e->is_moving = false;
        e->is_visible = true;
        e->is_solid = true;

        e->has_glow = false;
        e->glow_r = 1.0f; 
        e->glow_g = 1.0f; 
        e->glow_b = 1.0f;

        e->uv_speed_x = 0.0f;
        e->uv_speed_y = 0.0f;

        e->attached_to_camera = false;
        
        return free_slot;
    }

    return -1;
}

void set_entity_scale(Scene* scene, int entity_id, float sx, float sy, float sz) {
    if (entity_id >= 0 && entity_id < scene->entity_count) {
        scene->entities[entity_id].sx = sx;
        scene->entities[entity_id].sy = sy;
        scene->entities[entity_id].sz = sz;
    }
}

void set_entity_collider_scale(Scene* scene, int entity_id, float cx, float cy, float cz) {
    if (entity_id >= 0 && entity_id < scene->entity_count) {
        scene->entities[entity_id].cx = cx;
        scene->entities[entity_id].cy = cy;
        scene->entities[entity_id].cz = cz;
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

void set_entity_uv_speed(Scene* scene, int entity_id, float speed_x, float speed_y) {
    if (entity_id >= 0 && entity_id < scene->entity_count) {
        scene->entities[entity_id].uv_speed_x = speed_x;
        scene->entities[entity_id].uv_speed_y = speed_y;
    }
}

void render_scene(const Scene* scene, Camera* camera, Shader* shader) {
    bind_lights_to_shader(scene, shader);

    for (int i = 0; i < scene->entity_count; i++) {
        const Entity* e = &(scene->entities[i]);

        if (!e->is_active) {
            continue;
        }
        if (!e->is_visible && !show_colliders) {
            continue;
        }
        
        if (!e->attached_to_camera) {
            float dx = e->x - camera->position[0];
            float dy = e->y - camera->position[1];
            float dz = e->z - camera->position[2];
            
            if ((dx * dx + dy * dy + dz * dz) > (300.0f * 300.0f)) {
                continue; 
            }
        }

        mat4 model;
        if (e->attached_to_camera) {
            glm_mat4_inv(camera->view_matrix, model);

            glm_translate(model, (vec3){e->attach_ox, e->attach_oy, e->attach_oz});
            
            glm_rotate(model, glm_rad(e->attach_rx), (vec3){1.0f, 0.0f, 0.0f});
            glm_rotate(model, glm_rad(e->attach_ry), (vec3){0.0f, 1.0f, 0.0f});
            glm_rotate(model, glm_rad(e->attach_rz), (vec3){0.0f, 0.0f, 1.0f});
            
            glm_scale(model, (vec3){e->sx, e->sy, e->sz});
        } else {
            glm_mat4_identity(model);
            glm_translate(model, (vec3){e->x, e->y, e->z});
            glm_rotate(model, glm_rad(e->rx), (vec3){1.0f, 0.0f, 0.0f});
            glm_rotate(model, glm_rad(e->ry), (vec3){0.0f, 1.0f, 0.0f});
            glm_rotate(model, glm_rad(e->rz), (vec3){0.0f, 0.0f, 1.0f});
            glm_scale(model, (vec3){e->sx, e->sy, e->sz});
        }
        
        glUniformMatrix4fv(glGetUniformLocation(shader->id, "model"), 1, GL_FALSE, (float*)model);
        glUniform3f(glGetUniformLocation(shader->id, "objScale"), e->sx, e->sy, e->sz);

        glUniform1i(glGetUniformLocation(shader->id, "u_has_glow"), e->has_glow);
        glUniform3f(glGetUniformLocation(shader->id, "u_glow_color"), e->glow_r, e->glow_g, e->glow_b);

        glUniform3f(glGetUniformLocation(shader->id, "specularColor"), e->spec_r, e->spec_g, e->spec_b);
        glUniform1f(glGetUniformLocation(shader->id, "shininess"), e->shininess);

        glUniform2f(glGetUniformLocation(shader->id, "u_uv_speed"), e->uv_speed_x, e->uv_speed_y);
        
        if (e->is_visible) {
            for (int s = 0; s < e->model->submesh_count; s++) {
                int mat_id = e->model->submeshes[s].material_id;
                GLuint tex = 0;
                
                if (e->texture_count == 1) {
                    tex = e->textures[0];
                } else if (mat_id >= 0 && mat_id < e->texture_count) {
                    tex = e->textures[mat_id];
                } else if (e->texture_count > 0) {
                    tex = e->textures[0];
                }

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex);
                glUniform1i(glGetUniformLocation(shader->id, "diffuse_texture"), 0);

                draw_model_submesh(e->model, s);
            }
        }

        if (show_colliders && e->is_solid) {
            render_debug_collider(e, shader);
        }
    }
}

void update_scene(Scene* scene, double elapsed_time) {
    for (int i = 0; i < scene->entity_count; i++) {
        Entity* e = &(scene->entities[i]);
        if (!e->is_active) {
            continue;
        }
        
        update_entity(e, (float)elapsed_time);
    }
}

void set_ambient_light(Scene* scene, float r, float g, float b) {
    scene->ambient.r = r; scene->ambient.g = g; scene->ambient.b = b;
}

void set_directional_light(Scene* scene, float dx, float dy, float dz, float r, float g, float b) {
    scene->directional_light.dir_x = dx; scene->directional_light.dir_y = dy; scene->directional_light.dir_z = dz;
    scene->directional_light.r = r; scene->directional_light.g = g; scene->directional_light.b = b;
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
    
    printf("ERROR: Max point lights reached.\n");

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
    
    printf("ERROR: Max spot lights reached.\n");

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
    glUniform3f(glGetUniformLocation(shader->id, "directionalLight.direction"), scene->directional_light.dir_x, scene->directional_light.dir_y, scene->directional_light.dir_z);
    glUniform3f(glGetUniformLocation(shader->id, "directionalLight.color"), scene->directional_light.r, scene->directional_light.g, scene->directional_light.b);
    
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
    if (obb_cube == NULL) {
        return;
    }

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

void init_skybox(Scene* scene) {
    glGenVertexArrays(1, &scene->skybox.vao);
    glGenBuffers(1, &scene->skybox.vbo);
    glBindVertexArray(scene->skybox.vao);
    glBindBuffer(GL_ARRAY_BUFFER, scene->skybox.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unit_cube_vertices), &unit_cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    load_shader(&scene->skybox.shader, "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
    
    const char* faces[6] = {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/back.jpg",
        "assets/textures/skybox/front.jpg"
    };
    scene->skybox.texture_id = load_cubemap(faces);
    scene->skybox.is_active = true;
}

void render_skybox(Scene* scene, const Camera* camera, mat4 projection_matrix) {
    if (!scene->skybox.is_active) return;

    glDepthMask(GL_FALSE);
    use_shader(&(scene->skybox.shader));
    
    mat4 view_rot;
    glm_mat4_copy((vec4*)camera->view_matrix, view_rot);

    view_rot[3][0] = 0.0f;
    view_rot[3][1] = 0.0f;
    view_rot[3][2] = 0.0f;
    
    glUniformMatrix4fv(glGetUniformLocation(scene->skybox.shader.id, "view"), 1, GL_FALSE, (float*)view_rot);
    glUniformMatrix4fv(glGetUniformLocation(scene->skybox.shader.id, "projection"), 1, GL_FALSE, (float*)projection_matrix);
    glUniform3f(glGetUniformLocation(scene->skybox.shader.id, "skyColor"), scene->sky_r, scene->sky_g, scene->sky_b);
    
    glBindVertexArray(scene->skybox.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, scene->skybox.texture_id);
    glUniform1i(glGetUniformLocation(scene->skybox.shader.id, "skybox"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    glDepthMask(GL_TRUE); 
}

void destroy_skybox(Scene* scene) {
    if (scene->skybox.is_active) {
        glDeleteProgram(scene->skybox.shader.id);
        glDeleteVertexArrays(1, &scene->skybox.vao);
        glDeleteBuffers(1, &scene->skybox.vbo);
        glDeleteTextures(1, &scene->skybox.texture_id);
        scene->skybox.is_active = false;
    }
}

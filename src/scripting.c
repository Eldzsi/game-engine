#include "scripting.h"
#include "sound.h"
#include "ui.h"
#include "texture.h"
#include "model.h"
#include "scene.h"
#include "particle.h"
#include "collision.h"
#include "utils.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

static lua_State* L;
static Camera* engine_camera = NULL;
static Scene* engine_scene = NULL;
static ParticleSystem* engine_ps = NULL;
static Terrain* engine_terrain = NULL;

static int l_set_sky_color(lua_State* L) {
    float r = (float)luaL_checknumber(L, 1);
    float g = (float)luaL_checknumber(L, 2);
    float b = (float)luaL_checknumber(L, 3);

    glClearColor(r, g, b, 1.0f);

    if (engine_scene) {
        engine_scene->sky_r = r;
        engine_scene->sky_g = g;
        engine_scene->sky_b = b;
    }

    lua_pushboolean(L, true);

    return 1;
}

static int l_get_camera_position(lua_State* L) {
    if (engine_camera != NULL) {
        lua_pushnumber(L, engine_camera->position[0]);
        lua_pushnumber(L, engine_camera->position[1]);
        lua_pushnumber(L, engine_camera->position[2]);

        return 3;
    }

    lua_pushboolean(L, false);

    return 1;
}

static int l_set_camera_position(lua_State* L) {
    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);
    float z = (float)luaL_checknumber(L, 3);

    if (engine_camera != NULL) {
        engine_camera->position[0] = x;
        engine_camera->position[1] = y;
        engine_camera->position[2] = z;
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_get_camera_rotation(lua_State* L) {
    if (engine_camera != NULL) {
        lua_pushnumber(L, engine_camera->rotation[0]);
        lua_pushnumber(L, engine_camera->rotation[1]);
        lua_pushnumber(L, engine_camera->rotation[2]);

        return 3;
    }
    
    lua_pushboolean(L, false);

    return 1;
}

static int l_set_camera_rotation(lua_State* L) {
    float rx = (float)luaL_checknumber(L, 1);
    float ry = (float)luaL_checknumber(L, 2);
    float rz = (float)luaL_checknumber(L, 3);

    if (engine_camera != NULL) {
        if (rx < -89.0f) rx = -89.0f;
        if (rx > 89.0f) rx = 89.0f;

        while (rz < 0.0f) rz += 360.0f;
        while (rz > 360.0f) rz -= 360.0f;

        engine_camera->rotation[0] = rx;
        engine_camera->rotation[1] = ry;
        engine_camera->rotation[2] = rz;
        
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_get_element_position(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    
    if (engine_scene != NULL && id >= 0 && id < engine_scene->entity_count) {
        Entity* e = &(engine_scene->entities[id]);
        if (e->is_active) {
            lua_pushnumber(L, e->x);
            lua_pushnumber(L, e->y);
            lua_pushnumber(L, e->z);

            return 3;
        }
    }

    lua_pushboolean(L, false);

    return 1; 
}

static int l_set_element_position(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float z = (float)luaL_checknumber(L, 4);
    
    if (engine_scene != NULL && id >= 0 && id < engine_scene->entity_count) {
        Entity* e = &(engine_scene->entities[id]);
        if (e->is_active) {
            e->x = x;
            e->y = y;
            e->z = z;
            
            if (e->is_moving) {
                e->is_moving = false;
            }
            
            lua_pushboolean(L, true);

            return 1;
        }
    }
    
    lua_pushboolean(L, false);

    return 1;
}

static int l_play_sound(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);
    bool looped = lua_toboolean(L, 2); 
    int channel = play_sound(filename, looped);
    if (channel != -1) {
        lua_pushinteger(L, channel);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_play_sound_3d(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);

    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float z = (float)luaL_checknumber(L, 4);

    bool looped = lua_toboolean(L, 5);

    int channel = play_sound_3d(filename, x, y, z, looped);
    if (channel != -1) {
        lua_pushinteger(L, channel);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_set_sound_max_distance(lua_State* L) {
    int channel = (int)luaL_checkinteger(L, 1);
    set_sound_max_distance(channel, (float)luaL_checknumber(L, 2));
    
    lua_pushboolean(L, true);

    return 1;
}

static int l_set_sound_volume(lua_State* L) {
    int channel = (int)luaL_checkinteger(L, 1);
    set_sound_volume(channel, (float)luaL_checknumber(L, 2));

    lua_pushboolean(L, true);

    return 1;
}

static int l_set_sound_position(lua_State* L) {
    int channel = (int)luaL_checkinteger(L, 1);
    set_sound_position(channel, (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4));

    lua_pushboolean(L, true);

    return 1;
}

static int l_get_sound_position(lua_State* L) {
    int channel = (int)luaL_checkinteger(L, 1);

    float x, y, z;
    if (get_sound_position(channel, &x, &y, &z)) {
        lua_pushnumber(L, x); 
        lua_pushnumber(L, y); 
        lua_pushnumber(L, z);

        return 3;
    }

    lua_pushboolean(L, false);

    return 1;
}

static int l_create_object(lua_State* L) {
    const char* modelname = luaL_checkstring(L, 1);
    
    const char* texturenames[MAX_MATERIALS];
    int texture_count = 0;

    if (lua_istable(L, 2)) {
        int len = lua_rawlen(L, 2);
        for (int i = 1; i <= len && i <= MAX_MATERIALS; i++) {
            lua_rawgeti(L, 2, i);
            texturenames[texture_count++] = lua_tostring(L, -1);
            lua_pop(L, 1);
        }
    } else {
        texturenames[0] = luaL_checkstring(L, 2);
        texture_count = 1;
    }

    float x = (float)luaL_checknumber(L, 3);          
    float y = (float)luaL_checknumber(L, 4);
    float z = (float)luaL_checknumber(L, 5);
    float rx = (float)luaL_checknumber(L, 6);
    float ry = (float)luaL_checknumber(L, 7);
    float rz = (float)luaL_checknumber(L, 8);
    float sx = (float)luaL_optnumber(L, 9, 1.0);
    float sy = (float)luaL_optnumber(L, 10, 1.0);
    float sz = (float)luaL_optnumber(L, 11, 1.0);

    if (engine_scene != NULL) {
        int actual_id = create_entity(engine_scene, modelname, texturenames, texture_count, x, y, z, rx, ry, rz, sx, sy, sz);
        
        if (actual_id != -1) {
            lua_pushinteger(L, actual_id);
            return 1;
        }
    }

    lua_pushboolean(L, false); 
    return 1;
}

static int l_destroy_element(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    
    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        Entity* e = &(engine_scene->entities[id]);
        
        e->is_active = false;
        e->model = NULL;
        for (int i = 0; i < e->texture_count; i++) {
            e->textures[i] = 0;
        }
        e->texture_count = 0;

        e->x = 0; e->y = 0; e->z = 0;
        e->is_moving = false;
        
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_move_object(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    float time_ms = (float)luaL_checknumber(L, 2);
    
    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        Entity* e = &(engine_scene->entities[id]);
        
        e->start_x = e->x; e->start_y = e->y; e->start_z = e->z;
        e->start_rx = e->rx; e->start_ry = e->ry; e->start_rz = e->rz;
        
        e->target_x = (float)luaL_checknumber(L, 3);
        e->target_y = (float)luaL_checknumber(L, 4);
        e->target_z = (float)luaL_checknumber(L, 5);
        
        e->target_rx = (float)luaL_optnumber(L, 6, e->rx);
        e->target_ry = (float)luaL_optnumber(L, 7, e->ry);
        e->target_rz = (float)luaL_optnumber(L, 8, e->rz);
        
        e->move_time = time_ms / 1000.0f;
        e->elapsed_move_time = 0.0f;
        e->is_moving = true;
        
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_stop_object(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        engine_scene->entities[id].is_moving = false;
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_is_object_moving(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        lua_pushboolean(L, engine_scene->entities[id].is_moving);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_set_object_size(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1); 
    float size = (float)luaL_checknumber(L, 2); 

    if (engine_scene != NULL && id >= 0 && id < engine_scene->entity_count) {
        set_entity_scale(engine_scene, id, size, size, size);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_set_element_collider_scale(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    float cx = (float)luaL_checknumber(L, 2);
    float cy = (float)luaL_checknumber(L, 3);
    float cz = (float)luaL_checknumber(L, 4);

    if (engine_scene) {
        set_entity_collider_scale(engine_scene, id, cx, cy, cz);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

static int l_set_object_glow(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args < 5) {
        lua_pushboolean(L, false);

        return 1;
    }

    int entity_id = (int)luaL_checkinteger(L, 1);
    bool has_glow = lua_toboolean(L, 2);
    float r = (float)luaL_checknumber(L, 3);
    float g = (float)luaL_checknumber(L, 4);
    float b = (float)luaL_checknumber(L, 5);

    if (engine_scene != NULL && entity_id >= 0 && entity_id < engine_scene->entity_count) {
        set_entity_glow(engine_scene, entity_id, has_glow, r, g, b);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_set_object_material(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    float sr = (float)luaL_checknumber(L, 2);
    float sg = (float)luaL_checknumber(L, 3);
    float sb = (float)luaL_checknumber(L, 4);
    float shininess = (float)luaL_checknumber(L, 5);
    
    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        set_entity_material(engine_scene, id, sr, sg, sb, shininess);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_set_object_uv_speed(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    float speed_x = (float)luaL_checknumber(L, 2);
    float speed_y = (float)luaL_checknumber(L, 3);

    if (engine_scene != NULL && id >= 0 && id < engine_scene->entity_count) {
        set_entity_uv_speed(engine_scene, id, speed_x, speed_y);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_set_ambient_light(lua_State* L) {
    if (engine_scene) {
        set_ambient_light(engine_scene, luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3));
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_set_directional_light(lua_State* L) {
    if (engine_scene) {
        set_directional_light(engine_scene, luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6));
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_create_point_light(lua_State* L) {
    if (engine_scene) {
        int id = add_point_light(engine_scene, 
            luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3),
            luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6),
            luaL_checknumber(L, 7), luaL_checknumber(L, 8), luaL_checknumber(L, 9)
        );
        
        if (id != -1) {
            lua_pushinteger(L, id);

            return 1; 
        }
    }

    lua_pushboolean(L, false);

    return 1;
}

static int l_set_point_light(lua_State* L) {
    if (engine_scene) {
        int id = (int)luaL_checkinteger(L, 1);
        
        if (id >= 0 && id < MAX_POINT_LIGHTS && engine_scene->point_lights[id].is_active) {
            PointLight* pl = &(engine_scene->point_lights[id]);

            if (lua_isnumber(L, 2)) pl->x = (float)lua_tonumber(L, 2);
            if (lua_isnumber(L, 3)) pl->y = (float)lua_tonumber(L, 3);
            if (lua_isnumber(L, 4)) pl->z = (float)lua_tonumber(L, 4);
            if (lua_isnumber(L, 5)) pl->r = (float)lua_tonumber(L, 5);
            if (lua_isnumber(L, 6)) pl->g = (float)lua_tonumber(L, 6);
            if (lua_isnumber(L, 7)) pl->b = (float)lua_tonumber(L, 7);
            
            lua_pushboolean(L, true);

            return 1;
        }
    }

    lua_pushboolean(L, false);

    return 1;
}

static int l_destroy_point_light(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    
    if (engine_scene && id >= 0 && id < engine_scene->point_light_count) {
        engine_scene->point_lights[id].is_active = false;
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_destroy_spot_light(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    
    if (engine_scene && id >= 0 && id < engine_scene->spot_light_count) {
        engine_scene->spot_lights[id].is_active = false;
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    
    return 1;
}

static int l_create_spot_light(lua_State* L) {
    if (engine_scene) {
        int id = add_spot_light(engine_scene, 
            (float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3),
            (float)luaL_checknumber(L, 4), (float)luaL_checknumber(L, 5), (float)luaL_checknumber(L, 6),
            (float)luaL_checknumber(L, 7), (float)luaL_checknumber(L, 8), (float)luaL_checknumber(L, 9),
            (float)luaL_checknumber(L, 10), (float)luaL_checknumber(L, 11),
            (float)luaL_checknumber(L, 12), (float)luaL_checknumber(L, 13), (float)luaL_checknumber(L, 14)
        );
        
        if (id != -1) {
            lua_pushinteger(L, id);

            return 1;
        }
    }

    lua_pushboolean(L, false);

    return 1;
}

static int l_set_spot_light(lua_State* L) {
    if (engine_scene) {
        int id = (int)luaL_checkinteger(L, 1);
        
        if (id >= 0 && id < MAX_SPOT_LIGHTS && engine_scene->spot_lights[id].is_active) {
            SpotLight* sl = &(engine_scene->spot_lights[id]);

            if (lua_isnumber(L, 2)) {
                sl->x = (float)lua_tonumber(L, 2);
            }
            if (lua_isnumber(L, 3)) {
                sl->y = (float)lua_tonumber(L, 3);
            }
            if (lua_isnumber(L, 4)) {
                sl->z = (float)lua_tonumber(L, 4);
            }

            if (lua_isnumber(L, 5)) {
                sl->dir_x = (float)lua_tonumber(L, 5);
            }
            if (lua_isnumber(L, 6)) {
                sl->dir_y = (float)lua_tonumber(L, 6);
            }
            if (lua_isnumber(L, 7)) {
                sl->dir_z = (float)lua_tonumber(L, 7);
            }

            if (lua_isnumber(L, 8)) {
                sl->r = (float)lua_tonumber(L, 8);
            }
            if (lua_isnumber(L, 9)) {
                sl->g = (float)lua_tonumber(L, 9);
            }
            if (lua_isnumber(L, 10)) {
                sl->b = (float)lua_tonumber(L, 10);
            }
            
            lua_pushboolean(L, true);

            return 1;
        }
    }

    lua_pushboolean(L, false);

    return 1;
}

static int l_get_screen_size(lua_State* L) {
    SDL_Window* win = SDL_GL_GetCurrentWindow();
    int w = 800, h = 600;
    if (win) {
        SDL_GetWindowSize(win, &w, &h);
    }
    
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);

    return 2; 
}

static int l_draw_rectangle(lua_State* L) {
    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);
    float w = (float)luaL_checknumber(L, 3);
    float h = (float)luaL_checknumber(L, 4);
    float r = (float)luaL_checknumber(L, 5);
    float g = (float)luaL_checknumber(L, 6);
    float b = (float)luaL_checknumber(L, 7);
    float a = (float)luaL_checknumber(L, 8);
    
    draw_rectangle(x, y, w, h, r, g, b, a);

    lua_pushboolean(L, true);

    return 1;
}

static int l_draw_image(lua_State* L) {
    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);
    float w = (float)luaL_checknumber(L, 3);
    float h = (float)luaL_checknumber(L, 4);

    const char* path = luaL_checkstring(L, 5);
    
    float r = (float)luaL_optnumber(L, 6, 1.0);
    float g = (float)luaL_optnumber(L, 7, 1.0);
    float b = (float)luaL_optnumber(L, 8, 1.0);
    float a = (float)luaL_optnumber(L, 9, 1.0);
    
    draw_image(x, y, w, h, path, r, g, b, a);

    lua_pushboolean(L, true);

    return 1;
}

static int l_show_cursor(lua_State* L) {
    bool show = lua_toboolean(L, 1);
    
    if (show) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    } else {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    
    lua_pushboolean(L, true);

    return 1;
}

static int l_draw_text(lua_State* L) {
    const char* text = luaL_checkstring(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    const char* font = luaL_checkstring(L, 4);
    int size = (int)luaL_checkinteger(L, 5);
    
    float r = (float)luaL_optnumber(L, 6, 1.0);
    float g = (float)luaL_optnumber(L, 7, 1.0);
    float b = (float)luaL_optnumber(L, 8, 1.0);
    float a = (float)luaL_optnumber(L, 9, 1.0);
    
    const char* align = luaL_optstring(L, 10, "left");

    draw_text(text, x, y, font, size, r, g, b, a, align);

    lua_pushboolean(L, true);

    return 1;
}

static int l_set_element_visible(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    bool visible = lua_toboolean(L, 2);
    
    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        engine_scene->entities[id].is_visible = visible;
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_set_element_collisions_enabled(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    bool solid = lua_toboolean(L, 2);
    
    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        engine_scene->entities[id].is_solid = solid;
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_set_element_rotation(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    
    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        Entity* e = &(engine_scene->entities[id]);
        if (e->is_active) {
            e->rx = (float)luaL_checknumber(L, 2);
            e->ry = (float)luaL_checknumber(L, 3);
            e->rz = (float)luaL_checknumber(L, 4);
            lua_pushboolean(L, true);

            return 1;
        }
    }
    
    lua_pushboolean(L, false);

    return 1;
}

static int l_get_element_rotation(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    
    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        Entity* e = &(engine_scene->entities[id]);
        if (e->is_active) {
            lua_pushnumber(L, e->rx);
            lua_pushnumber(L, e->ry);
            lua_pushnumber(L, e->rz);

            return 3;
        }
    }

    lua_pushboolean(L, false);

    return 1;
}

static int l_clear_scene(lua_State* L) {
    if (engine_scene) {
        for (int i = 0; i < engine_scene->entity_count; i++) {
            engine_scene->entities[i].is_active = false;
        }
        engine_scene->entity_count = 0;
        
        engine_scene->point_light_count = 0;
        engine_scene->spot_light_count = 0;

        if (engine_terrain && engine_terrain->vao != 0) {
            destroy_terrain(engine_terrain);
            engine_terrain->vao = 0;
            engine_terrain->heights = NULL;
        }
        
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_stop_all_sounds(lua_State* L) {
    Mix_HaltChannel(-1);
    lua_pushboolean(L, true);

    return 1;
}

static int l_stop_sound(lua_State* L) {
    int channel = (int)luaL_checkinteger(L, 1);
    Mix_HaltChannel(channel);
    lua_pushboolean(L, true);

    return 1;
}

static int l_show_colliders(lua_State* L) {
    bool show = lua_toboolean(L, 1);
    set_show_colliders(show);
    
    lua_pushboolean(L, true);

    return 1;
}

static int l_emit_particle(lua_State* L) {
    if (!engine_ps) {
        lua_pushboolean(L, false);

        return 1;
    }

    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);
    float z = (float)luaL_checknumber(L, 3);
    float vx = (float)luaL_checknumber(L, 4);
    float vy = (float)luaL_checknumber(L, 5);
    float vz = (float)luaL_checknumber(L, 6);
    float r = (float)luaL_optnumber(L, 7, 1.0);
    float g = (float)luaL_optnumber(L, 8, 1.0);
    float b = (float)luaL_optnumber(L, 9, 1.0);
    float a = (float)luaL_optnumber(L, 10, 1.0);
    float life = (float)luaL_optnumber(L, 11, 1.0);
    float size = (float)luaL_optnumber(L, 12, 1.0);

    emit_particle(engine_ps, (vec3){x, y, z}, (vec3){vx, vy, vz}, (vec4){r, g, b, a}, life, size);
    
    lua_pushboolean(L, true);

    return 1;
}

static int l_exit_game(lua_State* L) {
    (void)L;
    
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);

    lua_pushboolean(L, true);
    
    return 1;
}

static int l_clear_particles(lua_State* L) {
    (void)L;
    if (engine_ps) {
        clear_particles(engine_ps);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_processLineOfSight(lua_State* L) {
    if (!engine_scene) {
        lua_pushboolean(L, false);

        return 1;
    }

    vec3 start = {(float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3)};
    vec3 end = {(float)luaL_checknumber(L, 4), (float)luaL_checknumber(L, 5), (float)luaL_checknumber(L, 6)};

    vec3 hit_point;
    int hit_entity_id = -1;

    bool hit = raycast_scene(engine_scene, start, end, hit_point, &hit_entity_id);

    lua_pushboolean(L, hit);
    if (hit) {
        lua_pushnumber(L, hit_point[0]);
        lua_pushnumber(L, hit_point[1]);
        lua_pushnumber(L, hit_point[2]);
        lua_pushinteger(L, hit_entity_id); 

        return 5; 
    }
    
    return 1;
}

static int l_get_camera_forward(lua_State* L) {
    if (engine_camera != NULL) {
        double pitch = engine_camera->rotation[0] * (M_PI / 180.0);
        double yaw = engine_camera->rotation[2] * (M_PI / 180.0);

        float dirX = (float)(-cos(yaw) * cos(pitch));
        float dirY = (float)(-sin(yaw) * cos(pitch));
        float dirZ = (float)(sin(pitch));

        lua_pushnumber(L, dirX);
        lua_pushnumber(L, dirY);
        lua_pushnumber(L, dirZ);

        return 3;
    }

    lua_pushboolean(L, false);

    return 1;
}

void bind_particle_system(ParticleSystem* ps) {
    engine_ps = ps;
}

static int l_load_terrain(lua_State* L) {
    if (!engine_terrain) {
        lua_pushboolean(L, false);

        return 1;
    }

    const char* h_path = luaL_checkstring(L, 1);
    const char* t_path = luaL_checkstring(L, 2);
    float scale_xz = (float)luaL_checknumber(L, 3);
    float scale_y = (float)luaL_checknumber(L, 4);
    
    float tile_size = (float)luaL_optnumber(L, 5, 0.0f);

    if (engine_terrain->vao != 0) {
        destroy_terrain(engine_terrain);
    }

    bool success = init_terrain(engine_terrain, h_path, t_path, scale_xz, scale_y, tile_size);
    
    lua_pushboolean(L, success);
    
    return 1;
}

static int l_get_terrain_height(lua_State* L) {
    if (!engine_terrain) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    float x = (float)luaL_checknumber(L, 1);
    float z = (float)luaL_checknumber(L, 2);
    
    float y = get_terrain_height(engine_terrain, x, z);
    lua_pushnumber(L, y);

    return 1;
}

static int l_unload_terrain(lua_State* L) {
    if (engine_terrain && engine_terrain->vao != 0) {
        destroy_terrain(engine_terrain);
        engine_terrain->vao = 0;
        engine_terrain->heights = NULL;
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

static int l_toggle_control(lua_State* L) {
    const char* control = luaL_checkstring(L, 1);
    bool state = lua_toboolean(L, 2);

    if (!engine_camera) {
        lua_pushboolean(L, false);
        return 1;
    }

    if (strcmp(control, "forward") == 0) {
        engine_camera->enable_forward = state;
        if (!state && engine_camera->speed[1] > 0) engine_camera->speed[1] = 0.0f;
    } else if (strcmp(control, "backward") == 0) {
        engine_camera->enable_backward = state;
        if (!state && engine_camera->speed[1] < 0) engine_camera->speed[1] = 0.0f;
    } else if (strcmp(control, "left") == 0) {
        engine_camera->enable_left = state;
        if (!state && engine_camera->speed[0] < 0) engine_camera->speed[0] = 0.0f;
    } else if (strcmp(control, "right") == 0) {
        engine_camera->enable_right = state;
        if (!state && engine_camera->speed[0] > 0) engine_camera->speed[0] = 0.0f;
    } else if (strcmp(control, "jump") == 0) {
        engine_camera->enable_jump = state;
    } else if (strcmp(control, "sprint") == 0) {
        engine_camera->enable_sprint = state;
        if (!state) engine_camera->is_sprinting = false;
    } else if (strcmp(control, "crouch") == 0) {
        engine_camera->enable_crouch = state;
        if (!state) engine_camera->is_crouching = false;
    } else {
        printf("WARNING: Unknown control '%s' in toggleControl\n", control);
        lua_pushboolean(L, false);
        return 1;
    }

    lua_pushboolean(L, true);
    return 1;
}

static int l_set_control_state(lua_State* L) {
    const char* control = luaL_checkstring(L, 1);
    bool state = lua_toboolean(L, 2);

    if (!engine_camera) {
        lua_pushboolean(L, false);
        return 1;
    }

    if (strcmp(control, "forward") == 0) {
        engine_camera->speed[1] = state ? 1.0f : 0.0f;
    } else if (strcmp(control, "backward") == 0) {
        engine_camera->speed[1] = state ? -1.0f : 0.0f;
    } else if (strcmp(control, "left") == 0) {
        engine_camera->speed[0] = state ? -1.0f : 0.0f;
    } else if (strcmp(control, "right") == 0) {
        engine_camera->speed[0] = state ? 1.0f : 0.0f;
    } else if (strcmp(control, "jump") == 0) {
        if (state) camera_jump(engine_camera);
    } else if (strcmp(control, "sprint") == 0) {
        engine_camera->is_sprinting = state;
    } else if (strcmp(control, "crouch") == 0) {
        engine_camera->is_crouching = state;
    } else {
        printf("WARNING: Unknown control '%s' in setControlState\n", control);
        lua_pushboolean(L, false);
        return 1;
    }

    lua_pushboolean(L, true);

    return 1;
}

static int l_attach_to_camera(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    float ox = (float)luaL_checknumber(L, 2);
    float oy = (float)luaL_checknumber(L, 3);
    float oz = (float)luaL_checknumber(L, 4);
    float rx = (float)luaL_optnumber(L, 5, 0);
    float ry = (float)luaL_optnumber(L, 6, 0);
    float rz = (float)luaL_optnumber(L, 7, 0);

    if (engine_scene && id >= 0 && id < engine_scene->entity_count) {
        Entity* e = &engine_scene->entities[id];
        e->attached_to_camera = true;
        e->attach_ox = ox; e->attach_oy = oy; e->attach_oz = oz;
        e->attach_rx = rx; e->attach_ry = ry; e->attach_rz = rz;
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

static int l_get_cursor_position(lua_State* L) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    
    return 2;
}

static int l_clear_engine_caches(lua_State* L) {
    clear_model_cache();
    clear_texture_cache();
    clear_sound_cache();
    clear_ui_cache();
    
    lua_pushboolean(L, true);

    return 1;
}

void trigger_lua_event(const char* event_name, const char* format, ...) {
    if (!L) return;
    lua_getglobal(L, "triggerEvent"); 
    
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        
        return;
    }

    lua_pushstring(L, event_name);
    int param_count = 1;

    if (format != NULL) {
        va_list args;
        va_start(args, format);
        
        for (int i = 0; format[i] != '\0'; i++) {
            if (format[i] == 's') {
                lua_pushstring(L, va_arg(args, char*));
            } else if (format[i] == 'i') {
                lua_pushinteger(L, va_arg(args, int));
            } else if (format[i] == 'f') {
                lua_pushnumber(L, va_arg(args, double));
            } else if (format[i] == 'b') {
                lua_pushboolean(L, va_arg(args, int));
            }
            param_count++;
        }
        va_end(args);
    }

    if (lua_pcall(L, param_count, 0, 0) != LUA_OK) { 
        printf("ERROR: triggerEvent(%s): %s\n", event_name, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

void init_scripting(Camera* camera, Scene* scene, Terrain* terrain) {
    engine_camera = camera;
    engine_scene = scene;
    engine_terrain = terrain;

    L = luaL_newstate();
    luaL_openlibs(L);

    lua_register(L, "setSkyColor", l_set_sky_color);
    lua_register(L, "getCameraPosition", l_get_camera_position);
    lua_register(L, "setCameraPosition", l_set_camera_position);
    lua_register(L, "getCameraRotation", l_get_camera_rotation);
    lua_register(L, "setCameraRotation", l_set_camera_rotation);
    lua_register(L, "getElementPosition", l_get_element_position);
    lua_register(L, "setElementPosition", l_set_element_position);
    lua_register(L, "playSound", l_play_sound);
    lua_register(L, "playSound3D", l_play_sound_3d);
    lua_register(L, "setSoundMaxDistance", l_set_sound_max_distance);
    lua_register(L, "setSoundVolume", l_set_sound_volume);
    lua_register(L, "setSoundPosition", l_set_sound_position);
    lua_register(L, "getSoundPosition", l_get_sound_position);
    lua_register(L, "createObject", l_create_object);
    lua_register(L, "destroyElement", l_destroy_element);
    lua_register(L, "moveObject", l_move_object);
    lua_register(L, "stopObject", l_stop_object);
    lua_register(L, "isObjectMoving", l_is_object_moving);
    lua_register(L, "setObjectSize", l_set_object_size);
    lua_register(L, "setElementColliderScale", l_set_element_collider_scale);
    lua_register(L, "setObjectGlow", l_set_object_glow);
    lua_register(L, "setObjectMaterial", l_set_object_material);
    lua_register(L, "setObjectUVSpeed", l_set_object_uv_speed);
    lua_register(L, "setAmbientLight", l_set_ambient_light);
    lua_register(L, "setDirectionalLight", l_set_directional_light);
    lua_register(L, "createPointLight", l_create_point_light);
    lua_register(L, "setPointLight", l_set_point_light);
    lua_register(L, "destroyPointLight", l_destroy_point_light);
    lua_register(L, "destroySpotLight", l_destroy_spot_light);
    lua_register(L, "createSpotLight", l_create_spot_light);
    lua_register(L, "setSpotLight", l_set_spot_light);
    lua_register(L, "getScreenSize", l_get_screen_size);
    lua_register(L, "drawRectangle", l_draw_rectangle);
    lua_register(L, "drawImage", l_draw_image);
    lua_register(L, "showCursor", l_show_cursor);
    lua_register(L, "drawText", l_draw_text);
    lua_register(L, "setElementVisible", l_set_element_visible);
    lua_register(L, "setElementCollisionsEnabled", l_set_element_collisions_enabled);
    lua_register(L, "setElementRotation", l_set_element_rotation);
    lua_register(L, "getElementRotation", l_get_element_rotation);
    lua_register(L, "clearScene", l_clear_scene);
    lua_register(L, "stopAllSounds", l_stop_all_sounds);
    lua_register(L, "stopSound", l_stop_sound);
    lua_register(L, "showColliders", l_show_colliders);
    lua_register(L, "emitParticle", l_emit_particle);
    lua_register(L, "exitGame", l_exit_game);
    lua_register(L, "clearParticles", l_clear_particles);
    lua_register(L, "processLineOfSight", l_processLineOfSight);
    lua_register(L, "getCameraForward", l_get_camera_forward);
    lua_register(L, "loadTerrain", l_load_terrain);
    lua_register(L, "getTerrainHeight", l_get_terrain_height);
    lua_register(L, "unloadTerrain", l_unload_terrain);
    lua_register(L, "toggleControl", l_toggle_control);
    lua_register(L, "setControlState", l_set_control_state);
    lua_register(L, "attachObjectToCamera", l_attach_to_camera);
    lua_register(L, "getCursorPosition", l_get_cursor_position);
    lua_register(L, "clearEngineCaches", l_clear_engine_caches);
    
    if (luaL_dofile(L, "lua_core/init.lua") != LUA_OK) {
        printf("ERROR: Lua script: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    } else {
        printf("INFO: Lua scripts loaded.\n");
    }
}

void close_scripting() {
    if (L) {
        lua_close(L);
        L = NULL;
    }
}

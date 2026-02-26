#include "scripting.h"
#include "sound.h"

#include <GL/gl.h>

#include <stdio.h>

static lua_State* L;
static Camera* engine_camera = NULL;
static Scene* engine_scene = NULL;

static int l_set_sky_color(lua_State* L);
static int l_set_camera_position(lua_State* L);
static int l_play_sound(lua_State* L);
static int l_create_object(lua_State* L);

void init_scripting(Camera* camera, Scene* scene) {
    engine_camera = camera;
    engine_scene = scene;

    L = luaL_newstate();
    luaL_openlibs(L);

    lua_register(L, "setSkyColor", l_set_sky_color);
    lua_register(L, "setCameraPosition", l_set_camera_position);
    lua_register(L, "playSound", l_play_sound);
    lua_register(L, "createObject", l_create_object);

    if (luaL_dofile(L, "assets/scripts/main.lua") != LUA_OK) {
        printf("Error: Lua script: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    } else {
        printf("Info: Lua script loaded\n");
    }
}

void trigger_lua_event(const char* event_name) {
    if (!L) return;

    lua_getglobal(L, "triggerEvent");

    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, event_name);

        if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
            printf("Error: Executing event (%s): %s\n", event_name, lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    } else {
        lua_pop(L, 1);
    }
}

static int l_set_sky_color(lua_State* L) {
    float r = (float)luaL_checknumber(L, 1);
    float g = (float)luaL_checknumber(L, 2);
    float b = (float)luaL_checknumber(L, 3);

    glClearColor(r, g, b, 1.0f);

    return 0;
}

static int l_set_camera_position(lua_State* L) {
    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);
    float z = (float)luaL_checknumber(L, 3);

    if (engine_camera != NULL) {
        engine_camera->position.x = x;
        engine_camera->position.y = y;
        engine_camera->position.z = z;
    }

    return 0;
}

static int l_play_sound(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);
    
    play_sound(filename);
    
    return 0;
}

static int l_create_object(lua_State* L) {
    const char* modelname = luaL_checkstring(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float z = (float)luaL_checknumber(L, 4);
    float rx = (float)luaL_checknumber(L, 5);
    float ry = (float)luaL_checknumber(L, 6);
    float rz = (float)luaL_checknumber(L, 7);

    if (engine_scene != NULL) {
        create_entity(engine_scene, modelname, x, y, z, rx, ry, rz);
    }

    return 0;
}

void close_scripting() {
    if (L) {
        lua_close(L);
    }
}

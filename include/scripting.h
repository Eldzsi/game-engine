#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "scene.h"
#include "camera.h"
#include "sound.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

void init_scripting(Camera* camera, Scene* scene);

void close_scripting();

void trigger_lua_event(const char* event_name);

#endif

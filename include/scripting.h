#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "scene.h"
#include "camera.h"
#include "sound.h"
#include "particle.h"
#include "terrain.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

void init_scripting(Camera* camera, Scene* scene, Terrain* terrain);
void close_scripting();
void trigger_lua_event(const char* event_name, const char* format, ...);
void bind_particle_system(ParticleSystem* ps);

#endif

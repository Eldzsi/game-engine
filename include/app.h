#ifndef APP_H
#define APP_H

#include "camera.h"
#include "scene.h"

#include <SDL2/SDL.h>

#include <stdbool.h>

typedef struct App
{
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool is_running;
    double uptime;
    Camera camera;
    Scene scene;
} App;

void init_app(App* app);

void init_opengl();

void reshape(GLsizei width, GLsizei height);

void handle_app_events(App* app);

SDL_bool is_key_pressed(SDL_Scancode key);

void update_app(App* app);

void render_app(App* app);

void destroy_app(App* app);

#endif

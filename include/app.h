#ifndef APP_H
#define APP_H

#include "camera.h"
#include "scene.h"
#include "shader.h"
#include "particle.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    GLuint vao, vbo;
    GLuint texture_id;
    Shader shader;
} Skybox;

typedef struct App {
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool is_running;
    double uptime;
    double delta_time;
    Camera camera;
    Scene scene;
    Shader base_shader;
    mat4 projection_matrix;
    ParticleSystem fire_ps;
    Skybox skybox;
} App;

void init_app(App* app);
void init_app(App* app);
void render_app(App* app);
void update_app(App* app);
void handle_app_events(App* app);
void destroy_app(App* app);

#endif

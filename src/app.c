#include "app.h"
#include "shader.h"
#include "scripting.h"
#include "ui.h"
#include "sound.h"
#include "model.h"

#include <glad/glad.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>

static void init_opengl();
static void reshape(App* app, int width, int height);
static SDL_bool is_key_pressed(SDL_Scancode key);
static void handle_keydown_event(App* app, SDL_KeyboardEvent* key);
static void handle_keyup_event(App* app, SDL_KeyboardEvent* key);

void init_app(App* app) {
    int error_code;
    int inited_loaders;
    int width, height;

    app->is_running = false;

    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0) {
        printf("ERROR: Failed to init SDL: %s\n", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_DisplayMode display_mode;
    if (SDL_GetCurrentDisplayMode(0, &display_mode) == 0) {
        width = display_mode.w;
        height = display_mode.h;
    } else {
        width = 800;
        height = 600;
    }

    app->window = SDL_CreateWindow(
        "Game - Modern OpenGL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL);
    
    if (app->window == NULL) {
        printf("ERROR: Failed to create window.\n");
        return;
    }

    inited_loaders = IMG_Init(IMG_INIT_PNG);
    if (inited_loaders == 0) {
        printf("ERROR: Failed to init IMG: %s\n", IMG_GetError());
        return;
    }

    if (TTF_Init() == -1) {
        printf("ERROR: Failed to init TTF: %s\n", TTF_GetError());
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL) {
        printf("ERROR: Failed to create OpenGL context.\n");
        return;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("ERROR: Failed to initialize GLAD.\n");
        return;
    }

    if (!load_shader(&(app->base_shader), "assets/shaders/base.vert", "assets/shaders/base.frag")) {
        printf("ERROR: Failed to load base shaders.\n");
        return;
    }

    init_opengl();
    reshape(app, width, height);

    init_model_cache();
    init_texture_cache();
    init_sound();
    init_camera(&(app->camera));
    init_scene(&(app->scene));
    init_scripting(&(app->camera), &(app->scene), &(app->scene.terrain));
    init_ui(width, height);
    init_particle_system(&(app->fire_ps), "assets/textures/fire.png");
    bind_particle_system(&(app->fire_ps));
    init_skybox(&(app->scene));

    app->scene.terrain.vao = 0;
    app->scene.terrain.heights = NULL;

    app->uptime = 0.0;
    app->delta_time = 0.0;

    app->is_running = true;
}

void render_app(App* app) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_skybox(&(app->scene), &(app->camera), app->projection_matrix);

    use_shader(&(app->base_shader));

    float time = (float)SDL_GetTicks() / 1000.0f;
    glUniform1f(glGetUniformLocation(app->base_shader.id, "u_time"), time);
    glUniformMatrix4fv(glGetUniformLocation(app->base_shader.id, "projection"), 1, GL_FALSE, (float*)app->projection_matrix);
    set_view_matrix(&(app->camera), &(app->base_shader));

    render_scene(&(app->scene), &(app->camera), &(app->base_shader));

    render_terrain(&(app->scene.terrain), &(app->base_shader));

    render_particles(&(app->fire_ps), app->camera.view_matrix, app->projection_matrix);

    trigger_lua_event("onRender", "sf", "root", app->delta_time);
    trigger_lua_event("onPostRender", "sf", "root", app->delta_time);

    SDL_GL_SwapWindow(app->window);
}

void update_app(App* app) {
    static Uint64 last_counter = 0;
    if (last_counter == 0) {
        last_counter = SDL_GetPerformanceCounter();
    }

    Uint64 current_counter = SDL_GetPerformanceCounter();

    Uint64 counter_delta = current_counter - last_counter;
    last_counter = current_counter;

    double elapsed_time = (double)counter_delta / (double)SDL_GetPerformanceFrequency();

    if (elapsed_time > 0.1) {
        elapsed_time = 0.1;
    }

    app->uptime += elapsed_time;
    app->delta_time = elapsed_time;

    if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
        update_scene(&(app->scene), elapsed_time);
        update_camera(&(app->camera), &(app->scene), &(app->scene.terrain), elapsed_time);
        update_sounds(&(app->camera));
        update_particles(&(app->fire_ps), elapsed_time);
    }
}

void handle_app_events(App* app) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                handle_keydown_event(app, &event.key);
                break;
            case SDL_KEYUP:
                handle_keyup_event(app, &event.key);
                break;
            case SDL_MOUSEMOTION:
                if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
                    rotate_camera(&(app->camera), event.motion.xrel, event.motion.yrel); 
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                trigger_lua_event("onClick", "sibii", "root", event.button.button, true, event.button.x, event.button.y);
                break;
            case SDL_MOUSEBUTTONUP:
                trigger_lua_event("onClick", "sibii", "root", event.button.button, false, event.button.x, event.button.y);
                break;
            case SDL_TEXTINPUT:
                trigger_lua_event("onTextInput", "ss", "root", event.text.text);
                break;
            case SDL_QUIT:
                app->is_running = false;
                break;
            default: break;
        }
    }
}

void destroy_app(App* app) {
    close_scripting();
    close_sound();
    free_model_cache();
    free_texture_cache();
    destroy_ui();
    destroy_particle_system(&(app->fire_ps));
    destroy_terrain(&(app->scene.terrain));

    destroy_skybox(&(app->scene));

    glDeleteProgram(app->base_shader.id);
    glDeleteTextures(1, &app->scene.skybox.texture_id);
    
    if (app->gl_context != NULL) SDL_GL_DeleteContext(app->gl_context);
    if (app->window != NULL) SDL_DestroyWindow(app->window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

static void handle_keydown_event(App* app, SDL_KeyboardEvent* key) {
    trigger_lua_event("onKey", "ssb", "root", SDL_GetScancodeName(key->keysym.scancode), true);

    if (SDL_GetRelativeMouseMode() == SDL_FALSE) return;
        
    switch (key->keysym.scancode) {
        case SDL_SCANCODE_W: 
            if (app->camera.enable_forward) {
                set_camera_speed(&(app->camera), 1);
            }
            break;
        case SDL_SCANCODE_S:
            if (app->camera.enable_backward) {
                set_camera_speed(&(app->camera), -1);
            }
            break;
        case SDL_SCANCODE_A:
            if (app->camera.enable_left) {
                set_camera_side_speed(&(app->camera), -1);
            }
            break;
        case SDL_SCANCODE_D:
            if (app->camera.enable_right) {
                set_camera_side_speed(&(app->camera), 1);
            }
            break;
        case SDL_SCANCODE_SPACE:
            if (app->camera.enable_jump) {
                if (app->camera.is_grounded) {
                    trigger_lua_event("onPlayerJump", "s", "root");
                }
                camera_jump(&(app->camera));
            }
            break;
        case SDL_SCANCODE_LCTRL:
            if (app->camera.enable_crouch) app->camera.is_crouching = true;
            break;
        case SDL_SCANCODE_LSHIFT:
            if (app->camera.enable_sprint) app->camera.is_sprinting = true;
            break;
        case SDL_SCANCODE_F1: {
            bool current_state = get_show_colliders();
            set_show_colliders(!current_state);
            break;
        }
        default:
            break;
    }
}

static void handle_keyup_event(App* app, SDL_KeyboardEvent* key) {
    trigger_lua_event("onKey", "ssb", "root", SDL_GetScancodeName(key->keysym.scancode), false);

   switch (key->keysym.scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_S: 
            if (is_key_pressed(SDL_SCANCODE_W) && app->camera.enable_forward) {
                set_camera_speed(&(app->camera), 1);
            }
            else if (is_key_pressed(SDL_SCANCODE_S) && app->camera.enable_backward) {
                set_camera_speed(&(app->camera), -1);
            }
            else {
                set_camera_speed(&(app->camera), 0);
            }
            break;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_D:
            if (is_key_pressed(SDL_SCANCODE_A) && app->camera.enable_left) {
                set_camera_side_speed(&(app->camera), -1); 
            }
            else if (is_key_pressed(SDL_SCANCODE_D) && app->camera.enable_right) {
                set_camera_side_speed(&(app->camera), 1);
            }
            else {
                set_camera_side_speed(&(app->camera), 0);
            }
            break;
        case SDL_SCANCODE_LCTRL:
            app->camera.is_crouching = false;
            break;
        case SDL_SCANCODE_LSHIFT:
            app->camera.is_sprinting = false;
            break;
        default: break;
    }
}

static void init_opengl() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

static void reshape(App* app, int width, int height) {
    glViewport(0, 0, width, height);
    
    float aspect = (float)width / (float)height;
    glm_perspective(glm_rad(60.0f), aspect, 0.1f, 1000.0f, app->projection_matrix);

    update_ui_projection(width, height);
}

static SDL_bool is_key_pressed(SDL_Scancode key) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    return state[key] != 0;
}

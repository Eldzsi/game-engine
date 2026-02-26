#include "app.h"
#include "scripting.h"

#include <SDL2/SDL_image.h>

#include <stdio.h>

void init_app(App* app) {
    int error_code;
    int inited_loaders;
    int width, height;

    app->is_running = false;

    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0) {
        printf("Error: SDL initialization: %s\n", SDL_GetError());
        return;
    }

    SDL_DisplayMode display_mode;
    if (SDL_GetCurrentDisplayMode(0, &display_mode) == 0) {
        width = display_mode.w;
        height = display_mode.h;
    } else {
        width = 800;
        height = 600;
    }

    app->window = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL);
    if (app->window == NULL) {
        printf("Error: Unable to create the window!\n");
        return;
    }

    inited_loaders = IMG_Init(IMG_INIT_PNG);
    if (inited_loaders == 0) {
        printf("Error: IMG init error: %s\n", IMG_GetError());
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL) {
        printf("Error: Unable to create the OpenGL context!\n");
        return;
    }

    init_opengl();
    reshape(width, height);

    init_camera(&(app->camera));
    init_scene(&(app->scene));

    app->is_running = true;

    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void init_opengl() {
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glClearColor(0.1, 0.1, 0.1, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    glClearDepth(1.0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void reshape(GLsizei width, GLsizei height) {
    double ratio = (double)width / height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glFrustum(-0.06 * ratio, 0.06 * ratio, -0.06, 0.06, 0.1, 100.0);
}

void handle_app_events(App* app) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                app->is_running = false;
                break;
            case SDL_SCANCODE_W: 
                set_camera_speed(&(app->camera), 1); 
                break;
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), -1);
                break;
            case SDL_SCANCODE_A:
                set_camera_side_speed(&(app->camera), 1);
                break;
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), -1);
                break;
            case SDL_SCANCODE_SPACE:
                if (app->camera.is_grounded) {
                    trigger_lua_event("onPlayerJump");
                }
                camera_jump(&(app->camera));
                break;
            case SDL_SCANCODE_LCTRL:
                app->camera.is_crouching = true; 
                break;
            case SDL_SCANCODE_LSHIFT:
                app->camera.is_sprinting = true;
                break;
            default: break;
        }
        break;

        case SDL_KEYUP:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_S: 
                if (is_key_pressed(SDL_SCANCODE_W)) {
                    set_camera_speed(&(app->camera), 1);
                } else if (is_key_pressed(SDL_SCANCODE_S)) {
                    set_camera_speed(&(app->camera), -1);
                } else {
                    set_camera_speed(&(app->camera), 0);
                }
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                if (is_key_pressed(SDL_SCANCODE_A)) {
                    set_camera_side_speed(&(app->camera), 1);
                } else if (is_key_pressed(SDL_SCANCODE_D)) {
                    set_camera_side_speed(&(app->camera), -1);
                } else {
                    set_camera_side_speed(&(app->camera), 0);
                }
                break;
            case SDL_SCANCODE_LCTRL:
                app->camera.is_crouching = false;
                break;
            case SDL_SCANCODE_LSHIFT:
                app->camera.is_sprinting = false;
                break;
            default: 
                break;
            }
            break;
        
        case SDL_MOUSEMOTION:
            rotate_camera(&(app->camera), -event.motion.xrel, -event.motion.yrel);
            break;

        case SDL_QUIT:
            app->is_running = false;
            break;

        default:
            break;
        }
    }
}

SDL_bool is_key_pressed(SDL_Scancode key) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    return state[key] != 0;
}

void update_app(App* app) {
    double current_time;
    double elapsed_time;

    current_time = (double)SDL_GetTicks() / 1000;
    elapsed_time = current_time - app->uptime;
    app->uptime = current_time;

    update_camera(&(app->camera), elapsed_time);
    update_scene(&(app->scene));
}

void render_app(App* app) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    set_view(&(app->camera));
    render_scene(&(app->scene));
    glPopMatrix();

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app) {
    if (app->gl_context != NULL) {
        SDL_GL_DeleteContext(app->gl_context);
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
    }

    SDL_Quit();
}

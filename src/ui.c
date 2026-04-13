#include "ui.h"
#include "texture.h"

#include <cglm/cglm.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <string.h>

#define MAX_CACHED_TEXTS 100
#define MAX_UI_FONTS 10

typedef struct {
    char text[256];
    char font_path[256];
    int size;
    int width;
    int height;
    GLuint texture_id;
} CachedText;

typedef struct {
    char path[256];
    int size;
    TTF_Font* font;
} UIFont;

static GLuint ui_vao, ui_vbo;
static Shader ui_shader;
static mat4 ui_projection;

static CachedText text_cache[MAX_CACHED_TEXTS];
static int text_cache_count = 0;

static UIFont ui_fonts[MAX_UI_FONTS];
static int ui_font_count = 0;

static CachedText* get_cached_text(const char* text, const char* font_path, int size);
static void draw_quad(float x, float y, float width, float height, float r, float g, float b, float a, bool use_tex, GLuint tex);
static TTF_Font* get_ui_font(const char* path, int size);

void init_ui(int screen_width, int screen_height) {
    if (!load_shader(&ui_shader, "assets/shaders/ui.vert", "assets/shaders/ui.frag")) {
        printf("ERROR: Failed to load UI shaders.\n");
    }

    float unit_quad_vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &ui_vao);
    glGenBuffers(1, &ui_vbo);

    glBindVertexArray(ui_vao);
    glBindBuffer(GL_ARRAY_BUFFER, ui_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unit_quad_vertices), unit_quad_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    update_ui_projection(screen_width, screen_height);
}

void update_ui_projection(int screen_width, int screen_height) {
    glm_ortho(0.0f, (float)screen_width, (float)screen_height, 0.0f, -1.0f, 1.0f, ui_projection);
}

void draw_rectangle(float x, float y, float width, float height, float r, float g, float b, float a) {
    draw_quad(x, y, width, height, r, g, b, a, false, 0);
}

void draw_image(float x, float y, float width, float height, const char* image_path, float r, float g, float b, float a) {
    GLuint tex = get_texture(image_path);
    if (tex != 0) {
        draw_quad(x, y, width, height, r, g, b, a, true, tex);
    }
}

void draw_text(const char* text, float x, float y, const char* font_path, int size, float r, float g, float b, float a, const char* align) {
    CachedText* cached = get_cached_text(text, font_path, size);
    GLuint tex;
    float w, h;
    bool is_temporary = false;

    if (cached) {
        tex = cached->texture_id;
        w = (float)cached->width;
        h = (float)cached->height;
    } else {
        TTF_Font* font = get_ui_font(font_path, size);
        if (!font) {
            return;
        }

        SDL_Color color = {255, 255, 255, 255};
        SDL_Surface* raw_surface = TTF_RenderUTF8_Blended(font, text, color);
        if (!raw_surface) {
            return;
        }

        SDL_Surface* surface = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(raw_surface); 
        if (!surface) {
            return;
        }

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        w = (float)surface->w;
        h = (float)surface->h;
        is_temporary = true;

        if (text_cache_count < MAX_CACHED_TEXTS) {
            strncpy(text_cache[text_cache_count].text, text, 255);
            text_cache[text_cache_count].text[255] = '\0';
            
            strncpy(text_cache[text_cache_count].font_path, font_path, 255);
            text_cache[text_cache_count].font_path[255] = '\0';

            text_cache[text_cache_count].size = size;
            text_cache[text_cache_count].texture_id = tex;
            text_cache[text_cache_count].width = surface->w;
            text_cache[text_cache_count].height = surface->h;
            text_cache_count++;
            
            is_temporary = false;
        }
        
        SDL_FreeSurface(surface);
    }

    float draw_x = x;
    if (strcmp(align, "center") == 0) draw_x -= w / 2.0f;
    else if (strcmp(align, "right") == 0) draw_x -= w;

    draw_quad(draw_x, y, w, h, r, g, b, a, true, tex);

    if (is_temporary) {
        glDeleteTextures(1, &tex);
    }
}

void clear_text_cache() {
    for (int i = 0; i < text_cache_count; i++) {
        glDeleteTextures(1, &text_cache[i].texture_id);
    }
    text_cache_count = 0;
}

void destroy_ui() {
    clear_text_cache();
    
    for (int i = 0; i < ui_font_count; i++) {
        if (ui_fonts[i].font) {
            TTF_CloseFont(ui_fonts[i].font);
        }
    }
    ui_font_count = 0;

    glDeleteVertexArrays(1, &ui_vao);
    glDeleteBuffers(1, &ui_vbo);
    destroy_shader(&ui_shader);
}

static CachedText* get_cached_text(const char* text, const char* font_path, int size) {
    for (int i = 0; i < text_cache_count; i++) {
        if (strcmp(text_cache[i].text, text) == 0 &&
            strcmp(text_cache[i].font_path, font_path) == 0 &&
            text_cache[i].size == size) {
                
            return &text_cache[i];
        }
    }

    return NULL;
}

static void draw_quad(float x, float y, float width, float height, float r, float g, float b, float a, bool use_tex, GLuint tex) {
    use_shader(&ui_shader);
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, (float[]){x, y, 0.0f});
    glm_scale(model, (float[]){width, height, 1.0f});

    glUniformMatrix4fv(glGetUniformLocation(ui_shader.id, "projection"), 1, GL_FALSE, (float*)ui_projection);
    glUniformMatrix4fv(glGetUniformLocation(ui_shader.id, "model"), 1, GL_FALSE, (float*)model);
    glUniform4f(glGetUniformLocation(ui_shader.id, "color"), r, g, b, a);
    glUniform1i(glGetUniformLocation(ui_shader.id, "useTexture"), use_tex);

    if (use_tex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(glGetUniformLocation(ui_shader.id, "image"), 0);
    }

    glBindVertexArray(ui_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

static TTF_Font* get_ui_font(const char* path, int size) {
    for (int i = 0; i < ui_font_count; i++) {
        if (strcmp(ui_fonts[i].path, path) == 0 && ui_fonts[i].size == size) {
            return ui_fonts[i].font;
        }
    }

    if (ui_font_count < MAX_UI_FONTS) {
        TTF_Font* font = TTF_OpenFont(path, size);
        if (font) {
            strcpy(ui_fonts[ui_font_count].path, path);
            ui_fonts[ui_font_count].size = size;
            ui_fonts[ui_font_count].font = font;
            ui_font_count++;

            return font;
        } else {
            printf("ERROR: Error loading font: %s\n", TTF_GetError());
        }
    }

    return NULL;
}

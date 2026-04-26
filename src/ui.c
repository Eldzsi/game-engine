#include "ui.h"
#include "texture.h"

#include <cglm/cglm.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <string.h>

#define MAX_CACHED_TEXTS 50
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
    bool failed;
} UIFont;

static struct {
    GLint projection;
    GLint model;
    GLint color;
    GLint use_texture;
    GLint image;
} ui_uniforms;

static GLuint ui_vao, ui_vbo;
static Shader ui_shader;
static mat4 ui_projection;

static CachedText text_cache[MAX_CACHED_TEXTS];
static int text_cache_count = 0;
static int next_cache_index = 0;

static UIFont ui_fonts[MAX_UI_FONTS];
static int ui_font_count = 0;

static CachedText* get_cached_text(const char* text, const char* font_path, int size);
static void draw_quad(float x, float y, float width, float height, float r, float g, float b, float a, bool use_tex, GLuint tex);
static TTF_Font* get_ui_font(const char* path, int size);

void init_ui(int screen_width, int screen_height) {
    if (!load_shader(&ui_shader, "assets/shaders/ui.vert", "assets/shaders/ui.frag")) {
        printf("ERROR: Failed to load UI shaders.\n");
    }

    ui_uniforms.projection = glGetUniformLocation(ui_shader.id, "projection");
    ui_uniforms.model = glGetUniformLocation(ui_shader.id, "model");
    ui_uniforms.color = glGetUniformLocation(ui_shader.id, "color");
    ui_uniforms.use_texture = glGetUniformLocation(ui_shader.id, "useTexture");
    ui_uniforms.image = glGetUniformLocation(ui_shader.id, "image");

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
    if (text == NULL || text[0] == '\0') {
        return;
    }

    CachedText* cached = get_cached_text(text, font_path, size);
    GLuint tex;
    float w, h;

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

        if (text_cache[next_cache_index].texture_id != 0) {
            glDeleteTextures(1, &text_cache[next_cache_index].texture_id);
        }

        strncpy(text_cache[next_cache_index].text, text, 255);
        text_cache[next_cache_index].text[255] = '\0';
        strncpy(text_cache[next_cache_index].font_path, font_path, 255);
        text_cache[next_cache_index].font_path[255] = '\0';

        text_cache[next_cache_index].size = size;
        text_cache[next_cache_index].texture_id = tex;
        text_cache[next_cache_index].width = surface->w;
        text_cache[next_cache_index].height = surface->h;

        next_cache_index = (next_cache_index + 1) % MAX_CACHED_TEXTS;
        if (text_cache_count < MAX_CACHED_TEXTS) {
            text_cache_count++;
        }
        
        SDL_FreeSurface(surface);
    }

    float draw_x = x;
    if (strcmp(align, "center") == 0) {
        draw_x -= w / 2.0f;
    }
    else if (strcmp(align, "right") == 0) {
        draw_x -= w;
    }

    draw_quad(draw_x, y, w, h, r, g, b, a, true, tex);
}

void clear_text_cache() {
    for (int i = 0; i < MAX_CACHED_TEXTS; i++) {
        if (text_cache[i].texture_id != 0) {
            glDeleteTextures(1, &text_cache[i].texture_id);
            text_cache[i].texture_id = 0;
        }
    }
    text_cache_count = 0;
    next_cache_index = 0;
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
        if (text_cache[i].size == size &&
            strcmp(text_cache[i].text, text) == 0 &&
            strcmp(text_cache[i].font_path, font_path) == 0) {
            return &text_cache[i];
        }
    }
    return NULL;
}

void begin_ui_frame() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    use_shader(&ui_shader);

    glBindVertexArray(ui_vao);
}

void end_ui_frame() {
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

static void draw_quad(float x, float y, float width, float height, float r, float g, float b, float a, bool use_tex, GLuint tex) {
    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){x, y, 0.0f});
    glm_scale(model, (vec3){width, height, 1.0f});

    glUniformMatrix4fv(ui_uniforms.projection, 1, GL_FALSE, (float*)ui_projection);
    glUniformMatrix4fv(ui_uniforms.model, 1, GL_FALSE, (float*)model);
    glUniform4f(ui_uniforms.color, r, g, b, a);
    glUniform1i(ui_uniforms.use_texture, use_tex ? 1 : 0);

    if (use_tex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(ui_uniforms.image, 0);
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

static TTF_Font* get_ui_font(const char* path, int size) {
    for (int i = 0; i < ui_font_count; i++) {
        if (ui_fonts[i].size == size && strcmp(ui_fonts[i].path, path) == 0) {
            return ui_fonts[i].failed ? NULL : ui_fonts[i].font;
        }
    }

    if (ui_font_count < MAX_UI_FONTS) {
        TTF_Font* font = TTF_OpenFont(path, size);
        strncpy(ui_fonts[ui_font_count].path, path, 255);
        ui_fonts[ui_font_count].size = size;

        if (font) {
            ui_fonts[ui_font_count].font = font;
            ui_fonts[ui_font_count].failed = false;
            ui_font_count++;
            return font;
        } else {
            ui_fonts[ui_font_count].font = NULL;
            ui_fonts[ui_font_count].failed = true;
            ui_font_count++;
            printf("CRITICAL ERROR: Failed to load font: %s. Path might be wrong or file missing.\n", path);
        }
    }

    return NULL;
}

void clear_ui_cache() {
    clear_text_cache();
    
    for (int i = 0; i < ui_font_count; i++) {
        if (ui_fonts[i].font) {
            TTF_CloseFont(ui_fonts[i].font);
        }
        ui_fonts[i].font = NULL;
    }
    ui_font_count = 0;
    printf("INFO: UI text cache and fonts cleared.\n");
}

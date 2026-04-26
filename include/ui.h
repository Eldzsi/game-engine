#ifndef UI_H
#define UI_H

#include "shader.h"
#include <glad/glad.h>
#include <stdbool.h>

void init_ui(int screen_width, int screen_height);
void update_ui_projection(int screen_width, int screen_height);
void draw_rectangle(float x, float y, float width, float height, float r, float g, float b, float a);
void draw_image(float x, float y, float width, float height, const char* image_path, float r, float g, float b, float a);
void draw_text(const char* text, float x, float y, const char* font_path, int size, float r, float g, float b, float a, const char* align);
void begin_ui_frame();
void end_ui_frame();
void clear_text_cache();
void destroy_ui();
void clear_ui_cache();

#endif

#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL_mixer.h>

void init_sound();

void play_sound(const char* filename);

void close_sound();

#endif
#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "camera.h"

#define MAX_CHANNELS 32

void init_sound();
int play_sound(const char* filename, bool looped);
int play_sound_3d(const char* filename, float x, float y, float z, bool looped);

void set_sound_max_distance(int channel, float max_dist);
void set_sound_volume(int channel, float volume);
void set_sound_position(int channel, float x, float y, float z);
bool get_sound_position(int channel, float* x, float* y, float* z);

void update_sounds(Camera* camera);
void close_sound();

#endif

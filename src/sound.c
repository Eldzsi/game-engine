#include "sound.h"

#include <stdio.h>
#include <string.h>

#define MAX_SOUNDS 20

typedef struct {
    char filename[256];
    Mix_Chunk* chunk;
} Sound;

static Sound cache[MAX_SOUNDS];
static int sound_count = 0;

void init_sound() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Error: SDL_mixer initialization: %s\n", Mix_GetError());
    } else {
        printf("Info: SDL_mixer started\n");
    }

    sound_count = 0;
}

void play_sound(const char* filename) {
    for (int i = 0; i < sound_count; i++) {
        if (strcmp(cache[i].filename, filename) == 0) {
            Mix_PlayChannel(-1, cache[i].chunk, 0);
            return;
        }
    }

    if (sound_count < MAX_SOUNDS) {
        Mix_Chunk* sound_effect = Mix_LoadWAV(filename);
        if (sound_effect == NULL) {
            printf("Error: Failed to load sound: %s (%s)\n", filename, Mix_GetError());
            return;
        }
        
        strcpy(cache[sound_count].filename, filename);
        cache[sound_count].chunk = sound_effect;
        
        Mix_PlayChannel(-1, cache[sound_count].chunk, 0);
        
        sound_count++;
        printf("Info: Sound loaded: %s\n", filename);
    } else {
        printf("Error: Sound cache is full");
    }
}

void close_sound() {
    for (int i = 0; i < sound_count; i++) {
        Mix_FreeChunk(cache[i].chunk);
    }
    
    Mix_CloseAudio();
}

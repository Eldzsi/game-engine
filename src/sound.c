#include "sound.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_SOUNDS 100

typedef struct {
    char filename[256];
    Mix_Chunk* chunk;
} SoundCache;

typedef struct {
    bool is_active;
    bool is_3d;
    float x, y, z;
    float max_distance;
    float base_volume;
} Sound;

static SoundCache cache[MAX_SOUNDS];
static int cache_count = 0;
static Sound channels[MAX_CHANNELS];

static Mix_Chunk* get_chunk(const char* filename);
static void update_channel_3d(int i, Camera* camera);

void init_sound() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("ERROR: Failed to initialize SDL_mixer: %s\n", Mix_GetError());
    } else {
        Mix_AllocateChannels(MAX_CHANNELS);
        for (int i = 0; i < MAX_CHANNELS; i++) {
            channels[i].is_active = false;
        }
        printf("INFO: SDL_mixer started with %d channels.\n", MAX_CHANNELS);
    }
    cache_count = 0;
}

int play_sound(const char* filename, bool looped) {
    Mix_Chunk* chunk = get_chunk(filename);
    if (!chunk) {
        return -1;
    }

    int loops = looped ? -1 : 0;
    int channel = Mix_PlayChannel(-1, chunk, loops);
    if (channel != -1 && channel < MAX_CHANNELS) {
        channels[channel].is_active = true;
        channels[channel].is_3d = false;
        channels[channel].base_volume = 1.0f;
        Mix_Volume(channel, MIX_MAX_VOLUME);
        Mix_SetPosition(channel, 0, 0);

        return channel;
    }

    return -1;
}

int play_sound_3d(const char* filename, float x, float y, float z, bool looped) {
    int channel = play_sound(filename, looped);
    if (channel != -1) {
        channels[channel].is_3d = true;
        channels[channel].x = x;
        channels[channel].y = y;
        channels[channel].z = z;
        channels[channel].max_distance = 20.0f;
    }

    return channel;
}

void set_sound_max_distance(int channel, float max_dist) {
    if (channel >= 0 && channel < MAX_CHANNELS) {
        channels[channel].max_distance = max_dist;
    }
}

void set_sound_volume(int channel, float volume) {
    if (channel >= 0 && channel < MAX_CHANNELS) {
        if (volume < 0.0f) {
            volume = 0.0f;
        }
        if (volume > 1.0f) {
            volume = 1.0f;
        }
        channels[channel].base_volume = volume;
        if (!channels[channel].is_3d) {
            Mix_Volume(channel, (int)(volume * MIX_MAX_VOLUME));
        }
    }
}

void set_sound_position(int channel, float x, float y, float z) {
    if (channel >= 0 && channel < MAX_CHANNELS) {
        channels[channel].x = x;
        channels[channel].y = y;
        channels[channel].z = z;
    }
}

bool get_sound_position(int channel, float* x, float* y, float* z) {
    if (channel >= 0 && channel < MAX_CHANNELS && channels[channel].is_active) {
        *x = channels[channel].x;
        *y = channels[channel].y;
        *z = channels[channel].z;

        return true;
    }

    return false;
}

void update_sounds(Camera* camera) {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (channels[i].is_active) {
            if (!Mix_Playing(i)) {
                channels[i].is_active = false;
                continue;
            }

            if (channels[i].is_3d) {
                update_channel_3d(i, camera);
            }
        }
    }
}

void close_sound() {
    Mix_HaltChannel(-1); 
    
    for (int i = 0; i < cache_count; i++) {
        Mix_FreeChunk(cache[i].chunk);
    }
    
    Mix_CloseAudio();
}

static Mix_Chunk* get_chunk(const char* filename) {
    for (int i = 0; i < cache_count; i++) {
        if (strcmp(cache[i].filename, filename) == 0) {
            return cache[i].chunk;
        }
    }

    if (cache_count < MAX_SOUNDS) {
        Mix_Chunk* chunk = Mix_LoadWAV(filename);
        if (chunk) {
            strncpy(cache[cache_count].filename, filename, sizeof(cache[cache_count].filename) - 1);
            cache[cache_count].filename[sizeof(cache[cache_count].filename) - 1] = '\0';

            cache[cache_count].chunk = chunk;
            cache_count++;

            return chunk;
        }
    }

    return NULL;
}

static void update_channel_3d(int i, Camera* camera) {
    float dx = channels[i].x - camera->position[0];
    float dy = channels[i].y - camera->position[1];
    float dz = channels[i].z - camera->position[2];
    
    float dist = sqrtf(dx * dx + dy * dy + dz * dz);
    float volume_multiplier = 1.0f;
    
    if (channels[i].max_distance > 0.0f) {
        float norm_dist = dist / channels[i].max_distance;
        if (norm_dist > 1.0f) norm_dist = 1.0f;
        volume_multiplier = 1.0f - norm_dist; 
    }

    Mix_SetPosition(i, 0, 0);
    
    float final_volume = channels[i].base_volume * volume_multiplier;
    Mix_Volume(i, (int)(final_volume * MIX_MAX_VOLUME));
}

void clear_sound_cache() {
    for (int i = 0; i < cache_count; i++) {
        if (cache[i].chunk) {
            Mix_FreeChunk(cache[i].chunk);
        }
    }
    cache_count = 0;
    printf("INFO: Sound cache cleared.\n");
}

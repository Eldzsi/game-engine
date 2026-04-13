#ifndef UTILS_H
#define UTILS_H

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float red, green, blue;
} Color;

typedef struct {
    Color specular;
    float shininess;
} Material;

static inline double degree_to_radian(double degree) {
    return degree * M_PI / 180.0;
}

#endif
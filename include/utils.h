#ifndef UTILS_H
#define UTILS_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float x, y, z;
} vec3_t;

typedef struct {
    float red, green, blue;
} Color;

typedef struct {
    Color specular;
    float shininess;
} Material;

double degree_to_radian(double degree);

#endif

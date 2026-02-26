#ifndef UTILS_H
#define UTILS_H

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

typedef struct Color {
    float red;
    float green;
    float blue;
} Color;

typedef struct Material {
    struct Color ambient;
    struct Color diffuse;
    struct Color specular;
    float shininess;
} Material;

double degree_to_radian(double degree);

#endif

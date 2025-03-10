#ifndef SHADER_H
#define SHADER_H
#include "vector.h"
#include "tga.h"

typedef struct Shader{
    //vertex shader will take the ith face and nth vert
    struct Vec4f(*vertex_shader)(int i, int j);
    //the vertex shader will barcyenctric cordiantes and a color
    int(*fragment_shader)(struct Vec3f bar, struct TGAColor *color);
}Shader;

#endif




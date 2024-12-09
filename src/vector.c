#include "vector.h"
#include <math.h>


void swapV2i(struct Vec2i *v0, struct Vec2i *v1){
    v0->x ^= v1->x;
    v1->x ^= v0->x;
    v0->x ^= v1->x;

    v0->y ^= v1->y;
    v1->y ^= v0->y;
    v0->y ^= v1->y;
}

int distanceVi(struct Vec2i v0, struct Vec2i v1){
    return (int)sqrt((v0.x - v1.x)^2 + (v0.y - v1.y)^2);
}

struct Vec2i vector_add2i(struct Vec2i v0, struct Vec2i v1){
    int x = v0.x + v1.x;
    int y = v0.y + v1.y;
    struct Vec2i result = {x, y};
    return result;
}


struct Vec2i vector_sub2i(struct Vec2i v0, struct Vec2i v1){
    int x = v0.x - v1.x;
    int y = v0.y - v1.y;
    struct Vec2i result = {x, y};
    return result;
}

struct Vec2i vector_scale2i(struct Vec2i v0, float scale){
    int x = v0.x * scale;
    int y = v0.y * scale;
    struct Vec2i result = {x, y};
    return result;
}

struct Vec3f vector_cross(struct Vec3f v0, struct Vec3f v1){
    float x = v0.y*v1.z - v0.z*v1.y;
    float y = v0.z*v1.x - v0.x*v1.z;
    float z = v0.x*v1.y - v0.y*v1.x;
    struct Vec3f result = {x, y, z};
    return result;
}


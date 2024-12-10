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
    return (int)sqrt((v0.x - v1.x)*(v0.x - v1.x) + (v0.y - v1.y)*(v0.y - v1.y));
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

struct Vec3f vector_sub3f(struct Vec3f v0, struct Vec3f v1){
    float x = v0.x - v1.x;
    float y = v0.y - v1.y;
    float z = v0.z - v1.z; struct Vec3f result = {x, y, z}; return result;

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

float dot(struct Vec3f v0, struct Vec3f v1){
     return v0.x*v1.x + v0.y*v1.y + v0.z * v1.z;
}

float magnitude3f(struct Vec3f v0){
    return sqrt((v0.x * v0.x) + (v0.y * v0.y) + (v0.z * v0.z)); 
}

void normalize3f(struct Vec3f *v0){
    float magnitude = magnitude3f(*v0);
    v0->x = v0->x/magnitude;
    v0->y = v0->y/magnitude;
    v0->z = v0->z/magnitude;
}


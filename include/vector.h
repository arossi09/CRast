#ifndef VECTOR_H
#define VECTOR_H

struct Vec3f{
    float x;
    float y;
    float z;
};

struct Vec2i{
    int x;
    int y;
};

struct Vec3i{
    int x;
    int y;
    int z;
};


void swapV2i(struct Vec2i *v0, struct Vec2i *v1);
int distanceV2i(struct Vec2i v0, struct Vec2i v1);
struct Vec2i vector_add2i(struct Vec2i v0, struct Vec2i v1);
struct Vec2i vector_sub2i(struct Vec2i v0, struct Vec2i v1);
struct Vec2i vector_scale2i(struct Vec2i v0, float scale);
struct Vec3f vector_cross(struct Vec3f v0, struct Vec3f v1);
struct Vec3f vector_sub3f(struct Vec3f v0, struct Vec3f v1);
float dot(struct Vec3f v0, struct Vec3f v1);
void normalize3f(struct Vec3f *v0);
float magnitude3f(struct Vec3f v0);

#endif

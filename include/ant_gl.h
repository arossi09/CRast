#ifndef ANT_GL_H
#define ANT_GL_H
#include "matrix.h"
#include "vector.h"
#include "tga.h"
#include "util.h"
#include "shader.h"


extern struct Mat4 ModelView;
extern struct Mat4 Viewport;
extern struct Mat4 Projection;

struct Vec3f barycentric(struct Vec2f A, struct Vec2f B, struct Vec2f C, struct Vec2f p);
void lookat(struct Vec3f eye, struct Vec3f center, struct Vec3f up);
void triangle(struct Vec4f *pts, struct TGA_image zbuffer, Shader shader,
        struct TGA_image image);
void texTriangle(struct Vec4f *pts, struct Vec3f *txtPts, float *zbuffer,
        struct TGA_image image, struct TGA_image diffuseText, float intensity);
void projection (float coeff);
void viewport(int x, int y, int w, int h );

//these should be in other library 
struct Mat4 multMat4Vec4(struct Mat4 m, struct Vec4f v); 
struct Vec4f m2v4(struct Mat4 m);






#endif

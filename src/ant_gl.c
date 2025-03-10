#include "../include/ant_gl.h"
#include <stdlib.h>
#include <float.h>
#include <math.h>

struct Mat4 ModelView;
struct Mat4 Viewport;
struct Mat4 Projection;
extern float varying_intensity_array[3];



struct Vec4f m2v4(struct Mat4 m){
    struct Vec4f res = {m.data[0][0]/m.data[3][0], m.data[1][0]/m.data[3][0], m.data[2][0]/m.data[3][0], 1};
    return res;
}

struct Mat4 multMat4Vec4(struct Mat4 m, struct Vec4f v) {
    struct Mat4 result = {0}; // Initialize all values to 0

    result.data[0][0] = m.data[0][0] * v.x + m.data[0][1] * v.y + m.data[0][2] * v.z + m.data[0][3] * v.w;
    result.data[1][0] = m.data[1][0] * v.x + m.data[1][1] * v.y + m.data[1][2] * v.z + m.data[1][3] * v.w;
    result.data[2][0] = m.data[2][0] * v.x + m.data[2][1] * v.y + m.data[2][2] * v.z + m.data[2][3] * v.w;
    result.data[3][0] = m.data[3][0] * v.x + m.data[3][1] * v.y + m.data[3][2] * v.z + m.data[3][3] * v.w;
    return result;
}

void projection (float coeff){
    Projection = identityMat4();
    Projection.data[3][2] =  coeff;
}


void lookat(struct Vec3f eye, struct Vec3f center, struct Vec3f up){
    struct Vec3f z = vector_sub3f(eye, center);
    normalize3f(&z);
    struct Vec3f x = vector_cross(up, z);
    normalize3f(&x);
    struct Vec3f y = vector_cross(z, x);
    normalize3f(&y);
    ModelView = identityMat4();

    ModelView.data[0][0] = x.x;
    ModelView.data[1][0] = y.x;
    ModelView.data[2][0] = z.x;

    ModelView.data[0][1] = x.y;
    ModelView.data[1][1] = y.y;
    ModelView.data[2][1] = z.y;

    ModelView.data[0][2] = x.z;
    ModelView.data[1][2] = y.z;
    ModelView.data[2][2] = z.z;

    ModelView.data[0][3] = -center.x;
    ModelView.data[1][3] = -center.y;
    ModelView.data[2][3] = -center.z;

}

void viewport(int x, int y, int w, int h ){
    Viewport = identityMat4();
    Viewport.data[0][3] = x+w/2.0f;
    Viewport.data[1][3] = y+h/2.0f;
    Viewport.data[2][3] = 255.f/2.0f;

    Viewport.data[0][0] = w/2.0f;
    Viewport.data[1][1] = h/2.0f;
    Viewport.data[2][2] = 255.f/2.0f;
}

//We need this function to generate the bary centric cordinates
//given three points to mak up a triangle and the point to be 
//tested.
struct Vec3f barycentric(struct Vec2f A, struct Vec2f B, struct Vec2f C, struct Vec2f P){
    // may be wrong
    struct Vec3f s[2];

    s[0].x = C.x - A.x;
    s[0].y = B.x - A.x;
    s[0].z = A.x - P.x;

    s[1].x = C.y - A.y;
    s[1].y = B.y - A.y;
    s[1].z = A.y - P.y;
    struct Vec3f u = vector_cross(s[0], s[1]);
    if(fabsf(u.z) > 1e-2){
        return (struct Vec3f){1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z};
    }
    return (struct Vec3f){-1, 1, 1};
}

void triangle(struct Vec4f *pts, struct TGA_image zbuffer, Shader shader,
        struct TGA_image image){

    struct Vec2f bboxmin = {FLT_MAX, FLT_MAX};
    struct Vec2f bboxmax = {FLT_MIN, FLT_MIN};
    //we need to create a box for the traingles points
    //to later loop through
    //
    for(int i = 0; i < 3; i++){
        bboxmin.x = min(bboxmin.x, pts[i].x/pts[i].w);
        bboxmin.y = min(bboxmin.y, pts[i].y/pts[i].w);


        bboxmax.x = max(bboxmax.x, pts[i].x/pts[i].w);
        bboxmax.y = max(bboxmax.y, pts[i].y/pts[i].w);
    }
    //we now loop through the boxes points so that we can
    //create a barycentric cordinate based on each point and determine
    //if it is in the triangle
    struct Vec3f P;
    struct TGAColor color;
    for(P.x = bboxmin.x; P.x <= bboxmax.x; P.x++){
        for(P.y = bboxmin.y; P.y <= bboxmax.y; P.y++){
            struct Vec3f bc_screen = barycentric((struct Vec2f){pts[0].x/pts[0].w, pts[0].y/pts[0].w},
                    (struct Vec2f) {pts[1].x/pts[1].w, pts[1].y/pts[1].w},
                    (struct Vec2f) {pts[2].x/pts[2].w, pts[2].y/pts[2].w},
                    (struct Vec2f) {P.x, P.y});
            float z = pts[0].z*bc_screen.x + pts[1].z*bc_screen.y + pts[2].z*bc_screen.z;
            float w = pts[0].w*bc_screen.x + pts[1].w*bc_screen.y + pts[2].w*bc_screen.z;
            int frag_depth = max(0, min(255, (int)z/w+.5));
            if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || getPixel(zbuffer, P.x, P.y).r > frag_depth)continue;

            int discard = shader.fragment_shader(bc_screen, &color);

            if(!discard){
                setPixel(zbuffer, P.x, P.y, (struct TGAColor){frag_depth});
                setPixel(image, P.x, P.y, color);
            }
        }
    }
}
void texTriangle(struct Vec4f *pts, struct Vec3f *txtPts, float *zbuffer,
        struct TGA_image image, struct TGA_image diffuseText, float intensity){
    struct TGAColor tex_color;
    struct Vec2f bboxmin = {FLT_MAX, FLT_MAX};
    struct Vec2f bboxmax = {FLT_MIN, FLT_MIN};
    //we need to create a box for the traingles points
    //to later loop through
    //
    for(int i = 0; i < 3; i++){
        bboxmin.x = min(bboxmin.x, pts[i].x/pts[i].w);
        bboxmin.y = min(bboxmin.y, pts[i].y/pts[i].w);


        bboxmax.x = max(bboxmax.x, pts[i].x/pts[i].w);
        bboxmax.y = max(bboxmax.y, pts[i].y/pts[i].w);
    }
    //we now loop through the boxes points so that we can
    //create a barycentric cordinate based on each point and determine
    //if it is in the triangle
    struct Vec3f P;
    for(P.x = bboxmin.x; P.x <= bboxmax.x; P.x++){
        for(P.y = bboxmin.y; P.y <= bboxmax.y; P.y++){

            struct Vec3f bc_screen = barycentric((struct Vec2f){pts[0].x/pts[0].w, pts[0].y/pts[0].w},
                    (struct Vec2f) {pts[1].x/pts[1].w, pts[1].y/pts[1].w},
                    (struct Vec2f) {pts[2].x/pts[2].w, pts[2].y/pts[2].w},
                    (struct Vec2f) {P.x, P.y});
            if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)continue;
            P.z = 0;
            P.z += pts[0].z*bc_screen.x;
            P.z += pts[1].z*bc_screen.y;
            P.z += pts[2].z*bc_screen.z;

            float u = 1.0f - (txtPts[0].x * bc_screen.x + txtPts[1].x * bc_screen.y + txtPts[2].x * bc_screen.z);
            float v = 1.0f - (txtPts[0].y * bc_screen.x + txtPts[1].y * bc_screen.y + txtPts[2].y * bc_screen.z);
            int tex_x = (int)(u*diffuseText.header.width);
            int tex_y = (int)(v*diffuseText.header.height);

            tex_color = getPixel(diffuseText, tex_x, tex_y);
            tex_color.r *= intensity;
            tex_color.g *= intensity;
            tex_color.b *= intensity;

            if(zbuffer[(int)(P.x+P.y*800)]<P.z){
                zbuffer[(int)(P.x+P.y*800)] = P.z;
                setPixel(image, P.x, P.y, tex_color);
            }
        }
    }
}

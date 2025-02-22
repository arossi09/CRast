#include "../include/ant_gl.h"
#include <stdlib.h>

struct Mat4 ModelView;
struct Mat4 Viewport;
struct Mat4 Projection;


struct Vec3f m2v(struct Mat4 m){
    struct Vec3f res = {m.data[0][0]/m.data[3][0], m.data[1][0]/m.data[3][0], m.data[2][0]/m.data[3][0]};
    return res;
}

struct Mat4 multMat4Vec4(struct Mat4 m, float x, float y, float z, float w) {
    struct Mat4 result = {0}; // Initialize all values to 0

    result.data[0][0] = m.data[0][0] * x + m.data[0][1] * y + m.data[0][2] * z + m.data[0][3] * w;
    result.data[1][0] = m.data[1][0] * x + m.data[1][1] * y + m.data[1][2] * z + m.data[1][3] * w;
    result.data[2][0] = m.data[2][0] * x + m.data[2][1] * y + m.data[2][2] * z + m.data[2][3] * w;
    result.data[3][0] = m.data[3][0] * x + m.data[3][1] * y + m.data[3][2] * z + m.data[3][3] * w;
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
struct Vec3f barycentric(struct Vec3f *pts, struct Vec3f p){
    //we need to take the cross product between ABx ACx PAx & ABy ACy PAy
    //in order to find the resulting orthogonal vector [u v 1]
    struct Vec3f x_vector= {pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - p.x};
    struct Vec3f y_vector= {pts[2].y - pts[0].y, pts[1].y -pts[0].y, pts[0].y-p.y};
    struct Vec3f u = vector_cross(x_vector, y_vector);
    struct Vec3f tmp = {-1, 1, 1};
    //if the z cordinate is < 1 then this means that the triangle is either a 
    //line or something else
    if (abs(u.z) < 1) { return tmp; };
    struct Vec3f result = {1.0f - (u.x + u.y)/u.z, u.y/u.z, u.x/u.z};
    return result;
}

void triangle(struct Vec3f *pts, float *zbuffer,
        struct TGA_image image, struct TGAColor color){

    struct Vec2i bboxmin = {800-1, 800-1};
    struct Vec2i bboxmax = {0, 0};
    struct Vec2i clamp = {800-1, 800-1};
    //we need to create a box for the traingles points
    //to later loop through
    for(int i = 0; i < 3; i++){
        bboxmin.x = max(0, min(bboxmin.x, pts[i].x));
        bboxmin.y = max(0, min(bboxmin.y, pts[i].y));


        bboxmax.x = max(clamp.x, min(bboxmax.x, pts[i].x));
        bboxmax.y = max(clamp.y, min(bboxmax.y, pts[i].y));
    }
    //we now loop through the boxes points so that we can
    //create a barycentric cordinate based on each point and determine
    //if it is in the triangle
    struct Vec3f P;
    for(P.x = bboxmin.x; P.x <= bboxmax.x; P.x++){
        for(P.y = bboxmin.y; P.y <= bboxmax.y; P.y++){
            struct Vec3f bc_screen = barycentric(pts, P);
            if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)continue;
            P.z = 0;
            P.z += pts[0].z*bc_screen.x;
            P.z += pts[1].z*bc_screen.y;
            P.z += pts[2].z*bc_screen.z;

            if(zbuffer[(int)(P.x+P.y*800)]<P.z){
                zbuffer[(int)(P.x+P.y*800)] = P.z;
                setPixel(image, P.x, P.y, color);
            }
        }
    }
}
void texTriangle(struct Vec3f *pts, struct Vec3f *txtPts, float *zbuffer,
        struct TGA_image image, struct TGA_image diffuseText, float intensity){
    struct Vec2i bboxmin = {800-1, 800-1};
    struct Vec2i bboxmax = {0, 0};
    struct Vec2i clamp = {800-1, 800-1};
    struct TGAColor tex_color;
    //we need to create a box for the traingles points
    //to later loop through
    for(int i = 0; i < 3; i++){
        bboxmin.x = max(0, min(bboxmin.x, pts[i].x));
        bboxmin.y = max(0, min(bboxmin.y, pts[i].y));


        bboxmax.x = max(clamp.x, min(bboxmax.x, pts[i].x));
        bboxmax.y = max(clamp.y, min(bboxmax.y, pts[i].y));
    }
    //we now loop through the boxes points so that we can
    //create a barycentric cordinate based on each point and determine
    //if it is in the triangle
    struct Vec3f P;
    for(P.x = bboxmin.x; P.x <= bboxmax.x; P.x++){
        for(P.y = bboxmin.y; P.y <= bboxmax.y; P.y++){
            struct Vec3f bc_screen = barycentric(pts, P);
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

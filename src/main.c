#include "tga.h"
#include "obj_loader.h"
#include "util.h"
#include "vector.h"
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 800
#define DEPTH 255

struct TGAColor white = {255, 255, 255};
struct TGAColor red = {255, 0, 0};
struct TGAColor blue = {0, 0, 255};
struct Vec3f camera = {0, 0, 3};


struct Vec3f m2v(struct Mat4 m){
    struct Vec3f res = {m.data[0][0]/m.data[3][0], m.data[1][0]/m.data[3][0], m.data[2][0]/m.data[3][0]};
    return res;
}


struct Mat4 viewport(int x, int y, int w, int h ){
    struct Mat4 m = identityMat4();
    m.data[0][3] = x+w/2.0f;
    m.data[1][3] = y+h/2.0f;
    m.data[2][3] = DEPTH/2.0f;

    m.data[0][0] = w/2.0f;
    m.data[1][1] = h/2.0f;
    m.data[2][2] = DEPTH/2.0f;
    return m;
}
struct Mat4 multMat4Vec4(struct Mat4 m, float x, float y, float z, float w) {
    struct Mat4 result = {0}; // Initialize all values to 0

    result.data[0][0] = m.data[0][0] * x + m.data[0][1] * y + m.data[0][2] * z + m.data[0][3] * w;
    result.data[1][0] = m.data[1][0] * x + m.data[1][1] * y + m.data[1][2] * z + m.data[1][3] * w;
    result.data[2][0] = m.data[2][0] * x + m.data[2][1] * y + m.data[2][2] * z + m.data[2][3] * w;
    result.data[3][0] = m.data[3][0] * x + m.data[3][1] * y + m.data[3][2] * z + m.data[3][3] * w;
    return result;
}

//Given two points, a TGA image and a color draws a line between the
//two points
void line(int x0, int y0, int x1, int y1, struct TGA_image image, 
        struct TGAColor color){
    int steep = 0;
    if(abs(x0-x1)<abs(y0-y1)){
        swapi(&x0, &y0);
        swapi(&x1, &y1);
        steep = 1;
    }
    if(x0>x1){
        swapi(&x0, &x1);
        swapi(&y0, &y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int D = abs(dy)*2;
    int error = 0;
    int y = y0;
    for(int x=x0; x<=x1; x++){
        if(steep){
            setPixel(image, y, x, color);
        }else{
            setPixel(image, x, y, color);
        }
        error += D;
        if(error > dx){
            y += (y1>y0?1:-1);
            error -= dx*2;
        }
    }
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

void triangle(struct Vec3f *pts, struct Vec3f *txtPts, float *zbuffer,
        struct TGA_image image, struct TGA_image diffuseText, float intensity){

    

    struct Vec2i bboxmin = {WIDTH-1, HEIGHT-1};
    struct Vec2i bboxmax = {0, 0};
    struct Vec2i clamp = {WIDTH-1, HEIGHT-1};
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

            if(zbuffer[(int)(P.x+P.y*WIDTH)]<P.z){
                zbuffer[(int)(P.x+P.y*WIDTH)] = P.z;
                setPixel(image, P.x, P.y, tex_color);
            }
        }
    }
}

int main(int argc, char* argv[]){
    struct OBJ_Model model;
    struct TGA_image diffuseText = loadTGA("tga/african_head_diffuse.tga");
    struct TGA_image image = createTGA(WIDTH, HEIGHT, RGB);
    model = loadModel("obj/african_head.obj");
    struct Vec3f light_dir = {0, 0, -1};
    float zbuffer[WIDTH*HEIGHT] = {INT_MIN};

    printf("Drawing Faces...\n");

    struct Mat4 Projection = identityMat4();
    struct Mat4 ViewPort = viewport(WIDTH/8, HEIGHT/8, WIDTH*3/4, HEIGHT*3/4);
    Projection.data[3][2] = -1.0f/camera.z;


    for(int i = 0; i < model.nfaces; i++){
        //draw triangles based off object file
        struct face face = model.faces[i];
        struct Vec3f screen_coords[3];
        struct Vec3f world_cords[3];
        struct Vec3f text_cords[3];
        for(int j = 0; j < 3;j++){
            //subtract 1 from index because it is relative
            struct Vec3f v = model.vertices[face.indices[j]-1];
            struct Vec3f vt= model.vtextures[face.vt_indices[j]-1];
            //convert the world cords to align properly in our view
            //probably want to change this soon so vector multiplcation is more clean
            screen_coords[j] = m2v(multMat4Vec4(multMat4(ViewPort, Projection), v.x, v.y, v.z, 1.0f));
            world_cords[j] = v;
            text_cords[j] = vt;
        }
        //get the normal vector by the cross of each side of the triangle
        struct Vec3f n = vector_cross(vector_sub3f(world_cords[2], world_cords[0]),
                vector_sub3f(world_cords[1], world_cords[0]));
        normalize3f(&n);

        //We need to get the intensity by the dot of the normal and light
        //direciton in the world to know how to color the triangle
        float intensity = dot(n, light_dir);
        if(intensity > 0){
            triangle(screen_coords, text_cords, zbuffer, image, diffuseText, intensity);
        }
    }

    writeTGA(image, "tga/outfile.tga", 0);
    free(image.pixel_bytes);
    free(diffuseText.pixel_bytes);
    freeObj(model);
    printf("DONE!\n");

    return 0;
    
}


/*
 * Depricated triangle 
void triangle(struct Vec2i *pts, 
        struct TGA_image image, struct TGAColor color){

    struct Vec2i bboxmin = {WIDTH-1, HEIGHT-1};
    struct Vec2i bboxmax = {0, 0};
    struct Vec2i clamp = {WIDTH-1, HEIGHT-1};
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
    struct Vec2i P;
    for(P.x = bboxmin.x; P.x <= bboxmax.x; P.x++){
        for(P.y = bboxmin.y; P.y <= bboxmax.y; P.y++){
            struct Vec3f bc_screen = barycentric(pts, P);
            if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)continue;
            setPixel(image, P.x, P.y, color);
        }
    }
}
 */

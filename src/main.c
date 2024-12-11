#include "tga.h"
#include "obj_loader.h"
#include "util.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define WIDTH 800
#define HEIGHT 800

struct TGAColor white = {255, 255, 255};
struct TGAColor red = {255, 0, 0};
struct TGAColor blue = {0, 0, 255};


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
    struct Vec3f P;
    for(P.x = bboxmin.x; P.x <= bboxmax.x; P.x++){
        for(P.y = bboxmin.y; P.y <= bboxmax.y; P.y++){
            struct Vec3f bc_screen = barycentric(pts, P);
            if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)continue;
            P.z = 0;
            P.z += pts[0].z*bc_screen.x;
            P.z += pts[1].z*bc_screen.y;
            P.z += pts[2].z*bc_screen.z;

            if(zbuffer[(int)(P.x+P.y*WIDTH)]<P.z){
                zbuffer[(int)(P.x+P.y*WIDTH)] = P.z;
                setPixel(image, P.x, P.y, color);
            }
        }
    }
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    int wireframe = 0;

    struct OBJ_Model model;

    if(2 == argc){
        model = loadModel(argv[1]);
    }
    else if(3 == argc){
        model = loadModel(argv[1]);
        wireframe = 1;
    }else{
        model = loadModel("obj/african_head.obj");
    }

    

    //struct TGA_image image = loadTGA(argv[1]);
    struct TGA_image image = createTGA(WIDTH, HEIGHT, RGB);


    struct Vec3f light_dir = {0, 0, -1};
    float zbuffer[WIDTH*HEIGHT] = {INT_MIN};
    if(!wireframe){
        for(int i = 0; i < model.nfaces; i++){
            //draw triangles based off object file
            struct face face = model.faces[i];
            struct Vec3f screen_coords[3];
            struct Vec3f world_cords[3];
            for(int j = 0; j < 3;j++){
                //subtract 1 from index because it is relative
                struct Vec3f v = model.vertices[face.indices[j]-1];
                //convert the world cords to align properly in our view
                struct Vec3f tmp = {(v.x+1)*WIDTH/2., (v.y+1)*HEIGHT/2,
                    (v.z+1)*( HEIGHT*WIDTH )/2};
                screen_coords[j] = tmp;
                world_cords[j] = v;
            }
            //get the normal vector by the cross of each side of the triangle
            struct Vec3f n = vector_cross(vector_sub3f(world_cords[2], world_cords[0]),
                    vector_sub3f(world_cords[1], world_cords[0]));
            normalize3f(&n);
            //We need to get the intensity by the dot of the normal and light
            //direciton in the world to know how to color the triangle
            float intensity = dot(n, light_dir);
            if(intensity > 0){
                struct TGAColor color = {255*intensity, 255* intensity, 255*intensity};
                triangle(screen_coords, zbuffer, image, color);
            }
        }
    }else{
        for(int i = 0; i < model.nfaces; i++){
            struct face face = model.faces[i];
            for(int j = 0; j < 3;j++){
                //subtract 1 from index because it is relative
                struct Vec3f v0 = model.vertices[face.indices[j]-1];
                //second index is moduloed because it will be 4 but needs
                //to be drawn to 1
                struct Vec3f v1 = model.vertices[face.indices[(j+1)%3]-1];
                //we need to add 1 to shift to [0, 2] range for pixel cordinates
                //0 is bottom left 1 is middle and 2 is the top right.
                //We multiply by width and height divided by 2 so multiplying
                //by the middle is the middle of the width and height and 
                //mutliplying by the top right cancels the 2 so its
                //just width and height.
                int x0 = (v0.x + 1.)*WIDTH/2;
                int y0 = (v0.y + 1.)*HEIGHT/2;
                int x1 = (v1.x + 1.)*WIDTH/2;
                int y1 = (v1.y + 1.)*HEIGHT/2;
                line(x0, y0, x1, y1, image, white);
            }
        }
    }

    writeTGA(image, "tga/outfile.tga", 0);

    free(image.pixel_bytes);
    freeObj(model);

    return 0;
}

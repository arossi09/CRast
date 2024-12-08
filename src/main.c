#include "tga.h"
#include "obj_loader.h"
#include "util.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 800

struct TGAColor white = {255, 255, 255};
struct TGAColor red = {255, 0, 0};
struct TGAColor blue = {0, 0, 255};


//Wrote my own TGA library that can read and write tga formatted files
//Used this along with another class I wrote to read .obj files into
//a data structure to be able to draw obj files to the tga file.


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



struct Vec3f barycentric(struct Vec2i *pts, struct Vec2i p){

   struct Vec3f temp1 = {pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - p.x};
   struct Vec3f temp2 = {pts[2].y - pts[0].y, pts[1].y -pts[0].y, pts[0].y-p.y};
   struct Vec3f u = vector_cross(temp1, temp2);
   struct Vec3f tmp = {-1, 1, 1};
   if (abs(u.z) < 1) { return tmp; };
   struct Vec3f result = {1.0f - (u.x + u.y)/u.z, u.y/u.z, u.x/u.z};
   return result;
}

void triangle(struct Vec2i *pts, 
        struct TGA_image image, struct TGAColor color){
    struct Vec2i bboxmin = {WIDTH-1, HEIGHT-1};
    struct Vec2i bboxmax = {0, 0};
    struct Vec2i clamp = {WIDTH-1, HEIGHT-1};
    for(int i = 0; i < 3; i++){
        bboxmin.x = max(0, min(bboxmin.x, pts[i].x));
        bboxmin.y = max(0, min(bboxmin.y, pts[i].y));


        bboxmax.x = max(clamp.x, min(bboxmax.x, pts[i].x));
        bboxmax.y = max(clamp.y, min(bboxmax.y, pts[i].y));
    }
    struct Vec2i P;
    for(P.x = bboxmin.x; P.x <= bboxmax.x; P.x++){
        for(P.y = bboxmin.y; P.y <= bboxmax.y; P.y++){
            struct Vec3f bc_screen = barycentric(pts, P);
            if(bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)continue;
            setPixel(image, P.x, P.y, color);
        }
    }
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    int wireframe = 0;

    struct OBJ_Model model;
    if(3 == argc){
        model = loadModel(argv[1]);
        wireframe = 1;
    }else{
        model = loadModel("obj/african_head.obj");
    }

    

    //struct TGA_image image = loadTGA(argv[1]);
    struct TGA_image image = createTGA(WIDTH, HEIGHT, RGB);

    if(!wireframe){
        for(int i = 0; i < model.nfaces; i++){
            //draw triangles based off object file
            struct face face = model.faces[i];
            struct Vec2i screen_coords[3];
            for(int j = 0; j < 3;j++){
                //subtract 1 from index because it is relative
                struct Vec3f world_coords = model.vertices[face.indices[j]-1];
                //convert the world cords to align properly in our view
                struct Vec2i tmp = {(world_coords.x+1)*WIDTH/2., (world_coords.y+1)*HEIGHT/2};
                screen_coords[j] = tmp;
            }
            struct TGAColor color = {rand()%255, rand()%255, rand()%255} ;
            triangle(screen_coords, image, color);
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

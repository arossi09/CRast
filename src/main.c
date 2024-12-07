#include "tga.h"
#include "obj_loader.h"
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 800

void swap(int *x, int *y){
    if(x == y) return;
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
}

void line(int x0, int y0, int x1, int y1, struct TGA_image image, 
        struct TGAColor color){
    int steep = 0;
    if(abs(x0-x1)<abs(y0-y1)){
        swap(&x0, &y0);
        swap(&x1, &y1);
        steep = 1;
    }
    if(x0>x1){
        swap(&x0, &x1);
        swap(&y0, &y1);
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
int main(int argc, char* argv[]){
    struct OBJ_Model model;
    if(2 == argc){
        model = loadModel(argv[1]);
    }else{
        model = loadModel("obj/african_head.obj");
    }

    struct TGAColor white = {255, 255, 255};

    //struct TGA_image image = loadTGA(argv[1]);
    struct TGA_image image = createTGA(WIDTH, HEIGHT, RGB);

    //draw triangles based off object file
    for(int i = 0; i < model.nfaces; i++){
        struct face face = model.faces[i];
        for(int j = 0; j < 3;j++){
            //subtract 1 from index because it is relative
            struct vertex v0 = model.vertices[face.indices[j]-1];
            //second index is moduloed because it will be 4 but needs
            //to be drawn to 1
            struct vertex v1 = model.vertices[face.indices[(j+1)%3]-1];
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


    writeTGA(image, "tga/outfile.tga", 0);

    free(image.pixel_bytes);
    freeObj(model);

    return 0;
}

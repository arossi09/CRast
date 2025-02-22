#include "../include/tga.h"
#include "../include/obj_loader.h"
#include "../include/vector.h"
#include "../include/matrix.h"
#include "../include/ant_gl.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define WIDTH 800
#define HEIGHT 800
#define DEPTH 255

struct TGAColor white = {255, 255, 255};
struct TGAColor red = {255, 0, 0};
struct TGAColor blue = {0, 0, 255};
struct Vec3f camera = {0, 0, 3};
struct Vec3f eye = {1, 1, 3};
struct Vec3f center = {0,0,0};
struct Vec3f up = {0, 1, 0};
struct Vec3f light_dir = {0, 0, -1};

int main(int argc, char* argv[]){


    if(argc < 2){
        printf("Usage: ./prog filename.obj [diffuse.tga]");
        return -1;
    }

    //load the diffuse texture if one was given
    struct TGA_image diffuseText;
    if(argc == 3){
        diffuseText = loadTGA(argv[2]);
    }


    //load the model and create the TGA
    struct OBJ_Model model;
    struct TGA_image image = createTGA(WIDTH, HEIGHT, RGB);
    model = loadModel(argv[1]);
    if(model.nverts == 0){
        return -1;
    }

    printf("Drawing Faces...\n");


    float zbuffer[WIDTH*HEIGHT] = {INT_MIN};
    lookat(eye, center, up);
    viewport(WIDTH/8, HEIGHT/8, WIDTH*3/4, HEIGHT*3/4);
    struct Vec3f z = vector_sub3f(eye, center);
    projection(Projection.data[3][2] = -1.0f/magnitude3f(z));

    //draw triangles based off object file
    for(int i = 0; i < model.nfaces; i++){
        struct face face = model.faces[i];
        struct Vec3f screen_coords[3];
        struct Vec3f world_cords[3];
        struct Vec3f text_cords[3];
        for(int j = 0; j < 3;j++){
            //subtract 1 from index because it is relative
            struct Vec3f v = model.vertices[face.indices[j]-1];
            struct Vec3f vt= model.vtextures[face.vt_indices[j]-1];

            //probably want to change this soon so vector multiplcation is more clean
            //Viewport * Projection * View * Model * v.
            screen_coords[j] = m2v(multMat4Vec4(multMat4(multMat4(Viewport, Projection), ModelView), v.x, v.y, v.z, 1.0f));
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
            struct TGAColor color = {255*intensity, 255* intensity, 255*intensity};
            if(argc == 3){
                texTriangle(screen_coords, text_cords, zbuffer, image, diffuseText, intensity);
            }else{
                triangle(screen_coords, zbuffer, image, color);
            }

        }
    }

    writeTGA(image, "outfile.tga", 0);
    free(image.pixel_bytes);
    free(diffuseText.pixel_bytes);
    freeObj(model);
    printf("DONE!\n");

    return 0;
    
}



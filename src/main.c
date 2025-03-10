#include "../include/tga.h"
#include "../include/obj_loader.h"
#include "../include/vector.h"
#include "../include/matrix.h"
#include "../include/ant_gl.h"
#include "../include/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <float.h>

#define WIDTH 800
#define HEIGHT 800
#define DEPTH 255

struct OBJ_Model model;
struct TGAColor white = {255, 255, 255};
struct TGAColor red = {255, 0, 0};
struct TGAColor blue = {0, 0, 255};
struct Vec3f eye = {1, 1, 3};
struct Vec3f center = {0,0,0};
struct Vec3f up = {0, 1, 0};
struct Vec3f light_dir = {0, 0, -1};


//need to create the vertex and fragment shader and imple,ment them in the 
//main loops
//
//
//need to alter hte triangle function to take a shader struct and use its
//fragment shader function to draw

float varying_intensity_array[3];
struct Vec4f diffuse_vertex_shader(int iface, int nvert){
        struct Vec3f v = model.vertices[model.faces[iface].indices[nvert]-1];
        struct Vec3f n = model.norm_vertices[model.faces[iface].n_indices[nvert]-1];
        n.z *= -1;
        n.x *= -1;
        n.y *= -1;
        normalize3f(&n);
        struct Vec4f gl_vert = m2v4(multMat4Vec4(multMat4(multMat4(Viewport,
                            Projection), ModelView), (struct Vec4f){v.x, v.y, v.z, 1.0f}));
        varying_intensity_array[nvert] = fmaxf(0, dot(n, light_dir));
        return gl_vert;        
}
int diffuse_fragment_shader(struct Vec3f bar, struct TGAColor *color){
    struct Vec3f varying_intensity;
    varying_intensity.x = varying_intensity_array[0];
    varying_intensity.y = varying_intensity_array[1];
    varying_intensity.z = varying_intensity_array[2];
    float intensity = dot(varying_intensity, bar) ;
    if(intensity < .1f){
        return 1;
    }
    *color = (struct TGAColor){255*intensity, 255*intensity, 255*intensity};
    return 0;
}


int toon_fragment_shader(struct Vec3f bar, struct TGAColor *color){
    struct Vec3f varying_intensity;
    varying_intensity.x = varying_intensity_array[0];
    varying_intensity.y = varying_intensity_array[1];
    varying_intensity.z = varying_intensity_array[2];
    float intensity = dot(varying_intensity, bar);
    if(intensity > .85) intensity = 1;
    else if(intensity > .60) intensity = .8;
    else if(intensity > .45) intensity = .6;
    else if(intensity > .30) intensity = .45;
    else if(intensity > .15) intensity = .30;
    else intensity = 0;
    *color = (struct TGAColor){255*intensity, 0, 0};
    return 0;
}



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
    struct TGA_image image = createTGA(WIDTH, HEIGHT, RGB);
    model = loadModel(argv[1]);
    if(model.nverts == 0){
        return -1;
    }

    printf("Drawing Faces...\n");
    float array_zbuffer[WIDTH*HEIGHT];
    struct TGA_image zbuffer = createTGA(WIDTH, HEIGHT, GRAYSCALE);
    for(int i = 0; i < WIDTH * HEIGHT; i++) array_zbuffer[i] = -FLT_MAX;
    lookat(eye, center, up);
    viewport(WIDTH/8, HEIGHT/8, WIDTH*3/4, HEIGHT*3/4);
    struct Vec3f z = vector_sub3f(eye, center);
    projection(-1.0f/magnitude3f(z));
    normalize3f(&light_dir);



    Shader shader = {
        .vertex_shader = diffuse_vertex_shader,
        .fragment_shader = toon_fragment_shader 
    };
    //draw triangles based off object file
    //loop through all the triangles
    for(int i = 0; i < model.nfaces; i++){
        struct face face = model.faces[i];
        struct Vec4f screen_coords[3];
        struct Vec3f world_cords[3];
        struct Vec3f text_cords[3];
        struct Vec3f n_cords[3];
        //loop through each vertex
        for(int j = 0; j < 3;j++){
            //subtract 1 from index because it is relative
            struct Vec3f v = model.vertices[face.indices[j]-1];
            struct Vec3f vt= model.vtextures[face.vt_indices[j]-1];
            struct Vec3f vn= model.norm_vertices[face.n_indices[j]-1];
            
            //Viewport * Projection * View * Model * v.
            screen_coords[j] = shader.vertex_shader(i, j);
            world_cords[j] = v;
            text_cords[j] = vt;
            n_cords[j] =  vn;
        }

        //get the normal vector by the cross of each side of the triangle
        struct Vec3f n = vector_cross(vector_sub3f(world_cords[2], world_cords[0]),
                vector_sub3f(world_cords[1], world_cords[0]));
        normalize3f(&n);

        //We need to get the intensity by the dot of the normal and light
        //direciton in the world to know how to color the triangle
        float intensity = dot(n, light_dir);
        if(intensity > 0){
            if(argc == 3){
                texTriangle(screen_coords, text_cords, array_zbuffer, image, diffuseText, intensity);
            }else{
                triangle(screen_coords, zbuffer, shader, image);
            }

        }
    }
    writeTGA(image, "outfile.tga", 0);
    writeTGA(zbuffer, "zbuffer.tga", 0);
    free(image.pixel_bytes);
    free(diffuseText.pixel_bytes);
    free(zbuffer.pixel_bytes);
    freeObj(model);
    printf("DONE!\n");
    return 0;
}

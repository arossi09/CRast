#include "obj_loader.h"
#include "util.h"
#include "vector.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>


//given a OBJ formated file loads the data into the OBJ_model struct
//which can be found in obj_loader.h 
struct OBJ_Model loadModel(const char *filename){
    printf("Loading object model...\n");
    struct OBJ_Model obj;

    int fd = open(filename, O_RDONLY);
    if(fd < 0){
        perror("loadModel: could not open file\n");
        return obj;
    }

    char line[BUFF];
    ssize_t verts_buff_size = BUFF;
    ssize_t vtexture_buff_size =BUFF;
    ssize_t faces_buff_size = BUFF;
    float x, y, z;
    float u, v, w;
    int v1, v2, v3;
    int vt1, vt2, vt3;
    obj.nverts = 0;
    obj.nvtexts = 0;
    obj.nfaces = 0;

    //need ot allocate vertices as a dynamic array of arrays to hold
    //vertices
    obj.vertices = (struct Vec3f*)malloc(verts_buff_size * sizeof(struct Vec3f)); 
    obj.vtextures = (struct Vec3f*)malloc(vtexture_buff_size * sizeof(struct Vec3f));
    obj.faces = (struct face*)malloc(faces_buff_size *sizeof(struct face));

    //read till EOF
    while(readLine(fd, line) > -1){
        //printLine(line);
        //we need to check if the beginning of the line 
        //is a vertex decleration
        if(strncmp(&line[0], "v ", 2) == 0){
            if(obj.nverts >= verts_buff_size){
                verts_buff_size*=2;
                struct Vec3f *temp = (struct Vec3f*)realloc(obj.vertices,
                        verts_buff_size*sizeof(struct Vec3f));
                if(temp == NULL){
                    printf("OBJ_model: coulndt realloc mem\n");
                    return obj;
                }
                obj.vertices = temp;
            }
            sscanf(line, "%*c %f %f %f", &x, &y, &z);
            struct Vec3f vert = {x, y, z};
            obj.vertices[obj.nverts] = vert;
            obj.nverts++;
        }    
        //for the vertex texture cordinates
        if(strncmp(&line[0], "vt ", 2) == 0){
            if(obj.nvtexts >= vtexture_buff_size){
                vtexture_buff_size *= 2;
                struct Vec3f *temp = (struct Vec3f*)realloc(obj.vtextures,
                        vtexture_buff_size*sizeof(struct Vec3f));
                if(temp == NULL){
                    printf("OBJ_model: couldnt realloc mem\n");
                    return obj;
                }
                obj.vtextures = temp;
            }
            sscanf(line, "%*c%*c  %f %f %f", &u, &v, &w);
            struct Vec3f vtext = {u, v, w};
            obj.vtextures[obj.nvtexts] = vtext;
            obj.nvtexts++;
        }
        //for the face indecies into the vertex array
        if(strncmp(&line[0], "f ", 2) == 0){
           if(obj.nfaces >= faces_buff_size){
               faces_buff_size*=2;
               struct face *temp = (struct face*)realloc(obj.faces,
                       faces_buff_size*sizeof(struct face));
               if(temp == NULL){
                   printf("OBJ_model: couldnt realloc mem\n");
                   return obj;
               }
               obj.faces = temp;
           } 
           
           //faces with texture cordinates
           if(sscanf(line, "%*c %d/%d/%*d %d/%d/%*d %d/%d/%*d", 
                       &v1, &vt1, &v2, &vt2, &v3, &vt3) == 6
                   || sscanf(line, "%*c %d/%d %d/%d %d/%d", 
                       &v1, &vt1, &v2, &vt2, &v3, &vt3) == 6){
               struct face face = {v1, v2, v3, vt1, vt2, vt3};
               face.indices[0] = v1;
               face.indices[1] = v2;
               face.indices[2] = v3;
               face.vt_indices[0] = vt1;
               face.vt_indices[1] = vt2;
               face.vt_indices[2] = vt3;
               obj.faces[obj.nfaces] = face;
               obj.nfaces++;
           }
           //faces without texture cordinates
           else if(sscanf(line, "%*c %d//%*d %d//%*d %d//%*d", &v1, &v2, &v3) == 3
                   || sscanf(line, "%*c %d %d %d", &v1, &v2, &v3) == 3){
               struct face face = {v1, v2, v3, vt1, vt2, vt3};
               face.indices[0] = v1;
               face.indices[1] = v2;
               face.indices[2] = v3;
               obj.faces[obj.nfaces] = face;
               obj.nfaces++;
           }
        }

    }

    close(fd);

    return obj;
}


//frees dynamic memory of a OBJ_Model struct
int freeObj(struct OBJ_Model obj){
    printf("Cleaning up OBJ models...\n");
    if(obj.nverts >= 1){
        free(obj.vertices);
    }
    if(obj.nfaces >= 1){
        free(obj.faces);
    }
    if(obj.nvtexts>= 1){
        free(obj.vtextures);
    }
    return 1;
}

#include "obj_loader.h"
#include "util.h"
#include "vector.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

struct OBJ_Model loadModel(const char *filename){
    struct OBJ_Model obj;

    int fd = open(filename, O_RDONLY);
    if(fd < 0){
        perror("loadModel: could not open file\n");
        return obj;
    }

    char line[BUFF];
    ssize_t verts_buff_size = BUFF;
    ssize_t faces_buff_size = BUFF;
    float x, y, z;
    int v1, v2, v3;
    obj.nverts = 0;
    obj.nfaces = 0;

    //need ot allocate vertices as a dynamic array of arrays to hold
    //vertices
    obj.vertices = (struct Vec3f*)malloc(verts_buff_size * sizeof(struct Vec3f)); 
    obj.faces = (struct face*)malloc(faces_buff_size *sizeof(struct face));

    //read till EOF
    while(readLine(fd, line) > -1){
        printLine(line);
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
           if(sscanf(line, "%*c %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &v1, &v2, &v3) == 3
                   || sscanf(line, "%*c %d %d %d", &v1, &v2, &v3) == 3
                   || sscanf(line, "%*c %d//%*d %d//%*d %d//%*d", &v1, &v2, &v3) == 3){
               struct face face = {v1, v2, v3};
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


int freeObj(struct OBJ_Model obj){
    if(obj.nverts >= 1){
        free(obj.vertices);
    }
    if(obj.nfaces >= 1){
        free(obj.faces);
    }
    return 1;
}

#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H 
#include "util.h"

#include <stdio.h>

#define BUFF 1024

struct face{
    int v1;
    int v2;
    int v3;
    int indices[3];
};

struct OBJ_Model{
    struct Vec3f *vertices;
    struct face *faces;
    int nverts;
    int nfaces;
};

//used for parsing the .obj file into struct data structure
struct OBJ_Model loadModel(const char *filename);


int freeObj(struct OBJ_Model obj);

#endif

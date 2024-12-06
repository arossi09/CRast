#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H 

#include <stdio.h>

#define BUFF 1024

struct OBJ_Model{
    unsigned char **vertices;
    unsigned char *faces;
    int nverts;
    int nfaces;
};

//used for parsing the .obj file into struct data structure
struct OBJ_Model loadModel(const char *filename);

#endif

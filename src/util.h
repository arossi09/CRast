#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

struct vertexf{
    float x;
    float y;
    float z;
};


struct Vert2i{
    int x;
    int y;
};

struct Vert3i{
    int x;
    int y;
    int z;
};

ssize_t readLine(int fd, char *buffer);
void printLine(char *line);
void swapi(int *x, int *y);
void swapv(struct Vert2i *v0, struct Vert2i *v1);
int distanceVi(struct Vert2i v0, struct Vert2i v1);
struct Vert2i vector_add2i(struct Vert2i v0, struct Vert2i v1);
struct Vert2i vector_sub2i(struct Vert2i v0, struct Vert2i v1);
struct Vert2i vector_scale2i(struct Vert2i v0, float scale);

#endif

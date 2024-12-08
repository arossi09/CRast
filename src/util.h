#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

struct vertexf{
    float x;
    float y;
    float z;
};


struct vertexi{
    int x;
    int y;
    int z;
};

ssize_t readLine(int fd, char *buffer);
void printLine(char *line);
void swapi(int *x, int *y);
void swapv(struct vertexi *v0, struct vertexi *v1);

#endif

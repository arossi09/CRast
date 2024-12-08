#include "util.h"
#include <unistd.h> 
#include <math.h>

//readline currently operates on the size of the buffer passed 
//which may be error prone
ssize_t readLine(int fd, char *buffer){
    ssize_t bytes_read = 0;
    ssize_t total_read = 0;

    while((bytes_read = read(fd, &buffer[total_read], 1))> 0){
        if(buffer[total_read] ==  '\n')
            break;
        total_read++;
    }

    //EOF
    if(bytes_read == 0)
        return -1;

    buffer[total_read] = '\0';
    return total_read;
}

void printLine(char *line){
    while(*line){
        printf("%c", *line);
        line++;
    }
    printf("\n");
}


void swapi(int *x, int *y){
    if(x == y) return;
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
}

void swapv(struct Vert2i *v0, struct Vert2i *v1){
    v0->x ^= v1->x;
    v1->x ^= v0->x;
    v0->x ^= v1->x;

    v0->y ^= v1->y;
    v1->y ^= v0->y;
    v0->y ^= v1->y;
}

int distanceVi(struct Vert2i v0, struct Vert2i v1){
    return (int)sqrt((v0.x - v1.x)^2 + (v0.y - v1.y)^2);
}

struct Vert2i vector_add2i(struct Vert2i v0, struct Vert2i v1){
    int x = v0.x + v1.x;
    int y = v0.y + v1.y;
    struct Vert2i result = {x, y};
    return result;
}


struct Vert2i vector_sub2i(struct Vert2i v0, struct Vert2i v1){
    int x = v0.x - v1.x;
    int y = v0.y - v1.y;
    struct Vert2i result = {x, y};
    return result;
}

struct Vert2i vector_scale2i(struct Vert2i v0, float scale){
    int x = v0.x * scale;
    int y = v0.y * scale;
    struct Vert2i result = {x, y};
    return result;
}

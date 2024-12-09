#include "util.h"
#include <unistd.h> 

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

int max(int x, int y){
   return x >= y ? x : y; 
}
int min(int x, int y){
    return x <= y ? x : y;
}

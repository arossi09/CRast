#include "util.h"
#include <unistd.h> 

ssize_t readLine(int fd, char *buffer){
    ssize_t bytes_read = 0;
    ssize_t total_read = 0;

    while((bytes_read = read(fd, &buffer[total_read], 1))> 0){
        if(bytes_read < 0)
            return -1;
        if(buffer[total_read] ==  '\n')
            break;
        total_read++;
    }

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

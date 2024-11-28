#include "tga.h"



struct TGA_header loadTGA(const char *filename){
    int fd = open(filename, O_RDONLY);

    struct TGA_header header;

    //check if the file descriptor was opened correctly
    if(fd < 0){
        perror(filename);
        printf("%d\n", fd);
        close(fd);
        exit(EXIT_FAILURE);
    }


    //we need to read each element of the header we need and 
    //store in the struct to return
    read(fd, header.ID, 1);
    read(fd, header.color_map_type, 1);
    read(fd, header.image_type, 1);
    lseek(fd, 5, SEEK_CUR);
    read(fd, header.image_specification, 1);

    close(fd);
    return header;
}

void printHeader(const struct TGA_header *header){
    printf("ID: %d\n", header->ID[0]);
    printf("Color Map Type: %d\n", header->color_map_type[0]);
    printf("Image Type: %d\n", header->image_type[0]);
    printf("Image Specification: %d\n", header->image_specification[0]);
}

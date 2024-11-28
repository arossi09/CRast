#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


//struct to hold the data of the 
//TGA header
struct TGA_header{
   char ID[1];
   char color_map_type[1];
   char image_type[1];
   char image_specification[10];
};

//loading image
struct TGA_header loadTGA(const char *filename);
int validateHeader(const struct TGA_header *header);


//saving image


//manipulation
void printHeader(const struct TGA_header *header);

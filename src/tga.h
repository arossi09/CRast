#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//struct to hold the data of the 
//TGA header
#pragma pack(push, 1)
struct TGA_header{
   unsigned char ID;
   unsigned char cm_type;
   unsigned char image_type;
   //color map specification 5 bytes
   unsigned short cm_start_index;
   unsigned short cm_length;
   unsigned char cm_entry_size;
   //image specification
   unsigned short x_origin;
   unsigned short y_origin;
   unsigned short width;
   unsigned short height;
   unsigned char pixel_depth;
   unsigned char image_descriptor;

};
#pragma pack(pop)

struct TGA_image{
    struct TGA_header header;
    unsigned char *pixel_bits;
};

//loading image
struct TGA_image loadTGA(const char *filename);
int validateHeader(const struct TGA_header *header);

//saving image


//manipulation
void printHeader(const struct TGA_header *header);




#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

typedef enum {
    GRAYSCALE=1,
    RGB=3,
    RGBA=4
}Depth;

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
    unsigned char *pixel_bytes;
    size_t size;
};

struct TGAColor{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};


//loading image and creatings
struct TGA_image loadTGA(const char *filename);
struct TGA_image createTGA(int width, int height, Depth format);
int validateHeader(const struct TGA_header *header);
int decode_RLE(unsigned char **data, size_t *size);

//saving image
int writeTGA(const struct TGA_image image, const char *filename, int rle);


//manipulation
void printHeader(const struct TGA_header *header);
int setPixel(struct TGA_image image, int x, int y, struct TGAColor color);
struct TGAColor getPixel(struct TGA_image image, int x, int y);







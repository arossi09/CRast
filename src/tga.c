#include "tga.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFF 1024
#define RW_PERM 0666


//given a TGA file path and a TGA_header struct
//loads the header data into the struct passed
struct TGA_image loadTGA(const char *filename){
    struct TGA_image image;

    int fd = open(filename, O_RDONLY);

    //check if the file descriptor was opened correctly
    if(fd < 0){
        perror(filename);
        printf("%d\n", fd);
        close(fd);
        exit(1);
    }

    //we neeed to create a buffer the size of struct to read
    //TGA file bytes into
    read(fd, &image.header, sizeof(struct TGA_header));
    
    if(!validateHeader(&image.header))
        exit(1);


    //we need to convert the bytes from the network to host for the shorts
    //because the file format is in little-endianess originally
    image.header.cm_start_index = ntohs(image.header.cm_start_index);
    image.header.cm_length = ntohs(image.header.cm_length);
    image.header.x_origin = ntohs(image.header.x_origin);
    image.header.y_origin = ntohs(image.header.y_origin);

    //we need to allocate memory for the pixel data
    int bytes_per_pixel = image.header.pixel_depth / 8;
    size_t data_size = image.header.width * image.header.height * bytes_per_pixel;
    image.pixel_bytes = (unsigned char*)malloc(data_size);
    image.size = data_size;



    if(!image.pixel_bytes){
        perror("failed to allocate memory for pixel bits.\n");
        close(fd);
        exit(1);
    }
  

    //check the image type to see if its compressed or not
    if(image.header.image_type == 2 || image.header.image_type == 3 ||
            image.header.image_type == 0){
        read(fd, image.pixel_bytes, data_size);
    }else if(image.header.image_type == 10){
        read(fd, image.pixel_bytes, data_size);
        decode_RLE(&image.pixel_bytes, &image.size);
    }


    //needs to read rest
    close(fd);

    return image;
}


//used for decoding RLE encoded format
//dont know if this works
int decode_RLE(unsigned char **data, size_t *size){
    size_t buffer_size = *size;
    unsigned char *decoded_data = (unsigned char*)malloc(*size);

    if(decoded_data == NULL){
        printf("decode_RLE: couldnt alocate memory\n");
        return 0;
    }

    int count;
    char character;
    int new_size = 0;

    for(int i = 0; i < *size-2; i+=2){
       count = *data[i] - 48; 
       character = *data[i+1];

       while(new_size + count > buffer_size){
           buffer_size *= 2;
           unsigned char *temp = (unsigned char*)realloc(decoded_data, buffer_size);
           if(temp == NULL){
               printf("decode_RLE: coulndt realloc mem\n");
               free(decoded_data);
               return 0;
           }
           decoded_data = temp;
       }

       for(int j = 0; j < count; j++){
           decoded_data[new_size] = character;
           new_size++;
       }
    }


    *data = realloc(decoded_data, new_size);
    *size = new_size;

    if(data == NULL){
        printf("decode_RLE, failed to realloc data\n");
        return 0;
    }

    return 1;
}

//validates headers to ensure they aren't corrupted
int validateHeader(const struct TGA_header *header){
    
    size_t bytes_per_pixel = header->pixel_depth/8;

    if(header->ID < 0 || header->ID > 255){
        printf("Validation: Invalid ID, %d\n", header->ID);
        return 0;
    }

    //we only except if there is or not a color image type for simplicity
    if(header->cm_type != 0 && header->cm_type != 1){
        printf("Validation: Invalid color image type, %d", header->cm_type);
        return 0;
    }

    //We need to validate color map types of only 2, 10, and 0 for simplicity
    //only allowing uncompressed true color images and 10, run-length encoded
    //true color images
    if(header->image_type != 2 && header->image_type != 3 &&
            header->image_type != 10  && header->image_type!= 0){
        printf("Validation: Invalid Color Map Type, %d\n", header->cm_type);
        return 0;
    }

    //check if width/height is invalid and if the picture isnt in greyscale,
    //rgb, or rgba format
    if(header->width < 0 || header->height < 0 ||
            (bytes_per_pixel != 1 && bytes_per_pixel != 3 && bytes_per_pixel != 4)){
        return 0;
    }
    return 1;

}



struct TGA_image createTGA(int width, int height, Depth format){
    struct TGA_image image;
    memset(&image.header, 0, sizeof(image.header));

    image.header.image_type = 2;
    image.header.width = width;
    image.header.height = height;
    image.header.pixel_depth = format * 8;
    image.size = width*height*format;
    image.pixel_bytes = calloc(image.size, sizeof(char));
    if(image.pixel_bytes == NULL){
        perror("createTGA: Could not allocate pixel data for image\n");
        exit(1);
    }

    return image;
}

//Used for saving image
int writeTGA(const struct TGA_image image, const char *filename, int rle){
    int fd = open(filename, O_RDWR | O_TRUNC | O_CREAT,  RW_PERM);

    size_t bytes_written = write(fd, &image.header, sizeof(image.header));

    if(bytes_written != sizeof(image.header)){
        perror("Failed to write header to outfile!\n");
        return -1;
    }


    bytes_written = write(fd, image.pixel_bytes, image.size);

    if(bytes_written != image.size){
        perror("Failed to write image to outfile!\n");
        return -1;
    }

    close(fd);
    return 1;
}


int setPixel(struct TGA_image image, int x, int y, struct TGAColor color){
    if(x < 0 || y < 0 || x >= image.header.width 
            || y >= image.header.height || image.size == 0){
        return -1;
    }

    //reverse order to account for TGA BGR format
    unsigned char raw[4];
    raw[0] = color.b;
    raw[1] = color.g;
    raw[2] = color.r;
    raw[3] = color.a;
    
    //we need to iterate jummp to positition and copy the color to it
    memcpy(image.pixel_bytes+(x+y*image.header.width)*image.header.pixel_depth/8,
            raw, image.header.pixel_depth/8);
    return 1;
}

struct TGAColor getPixel(struct TGA_image image, int x, int y){
    struct TGAColor pixel = {0, 0, 0, 255};
    if(x < 0 || y < 0 || x >= image.header.width 
            || y >= image.header.height || image.size == 0){
        return pixel;
    }
    int bpp = image.header.pixel_depth/8;
    int pos = (x+y*image.header.width)*bpp;

    char raw[bpp];
    int i = 0;
   
    for(int byte = pos+bpp-1; byte >= pos; byte--){
        raw[i] = image.pixel_bytes[byte];
        i++;
    }


    memcpy(&pixel, raw, bpp);
    return pixel;
}

void printHeader(const struct TGA_header *header){

    printf("%d x %d, %d\n", header->width, header->height,
            header->pixel_depth/8);
    printf("ID: %d\n", header->ID);
    printf("Color Image type: %d\n",header->cm_type);
    printf("Image Type: %d\n", header->image_type);
    printf("Color Image start index: %d\n", header->cm_start_index); 
    printf("Color Image length: %d\n", header->cm_length);
    printf("Color Image entry size: %d\n", header->cm_entry_size);
    printf("X origin: %d\n", header->x_origin);
    printf("Y origin: %d\n", header->y_origin);
    printf("Width: %d\n", header->width);
    printf("Height: %d\n", header->height);
    printf("Pixel depth: %d\n", header->pixel_depth);
    printf("Image descriptory: %d\n", header->image_descriptor);
}

#include "tga.h"
#include <stdio.h>

int main(int argc, char* argv[]){
    
    if(argc <= 1){
        printf("Usage prog [filename]\n");
        return -1;
    }


    //struct TGA_image image = loadTGA(argv[1]);
    struct TGA_image image = createTGA(100, 100, RGB);
    printHeader(&image.header);
  

    TGAColor red = {0, 0, 255};
    setPixel(image, 0, 0, red);
    writeTGA(image, "outfile.tga", 0);
    free(image.pixel_bytes);

    return 0;
}

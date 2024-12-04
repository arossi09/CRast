#include "tga.h"
#include <stdio.h>

// make image a typedef struct
void line(int x0, int y0, int x1, int y1, struct TGA_image image, TGAColor color){
    for(int x=x0; x<=x1; x++){
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        setPixel(image, x, y, color);
    }
}

int main(int argc, char* argv[]){
    
    if(argc <= 1){
        printf("Usage prog [filename]\n");
        return -1;
    }


    //struct TGA_image image = loadTGA(argv[1]);
    struct TGA_image image = createTGA(100, 100, RGB);
    printHeader(&image.header);
  

    TGAColor red = {255, 0, 0};
    TGAColor white = {255, 255, 255};

    line(30, 20, 80, 40, image, white);
    line(30, 20, 80, 80, image, red);


    writeTGA(image, "outfile.tga", 0);
    free(image.pixel_bytes);

    return 0;
}

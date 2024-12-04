#include "tga.h"

// make image a typedef struct
void line(int x0, int y0, int x1, int y1, struct TGA_image image, 
        struct TGAColor color){
    for(int x=x0; x<=x1; x++){
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        setPixel(image, x, y, color);
    }
}

int main(int argc, char* argv[]){


    //struct TGA_image image = loadTGA(argv[1]);
    struct TGA_image image = createTGA(100, 100, RGB);
    printHeader(&image.header);
  

    struct TGAColor red = {255, 0, 0};
    struct TGAColor white = {255, 255, 255};

    line(55, 70, 80, 60, image, red);
    line(50, 60, 55, 70, image, red);
    line(20, 60, 45, 70, image, red);
    line(45, 70, 50, 60, image, red);
    line(20, 60, 45, 70, image, red);
    line(20, 60, 50, 20, image, red);
    line(50, 20, 80, 60, image, red);


    writeTGA(image, "outfile.tga", 0);
    free(image.pixel_bytes);

    return 0;
}

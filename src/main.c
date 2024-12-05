#include "tga.h"


void swap(int *x, int *y){
    if(x == y) return;
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
}

void line(int x0, int y0, int x1, int y1, struct TGA_image image, 
        struct TGAColor color){
    int steep = 0;
    if(abs(x0-x1)<abs(y0-y1)){
        swap(&x0, &y0);
        swap(&x1, &y1);
        steep = 1;
    }
    if(x0>x1){
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int D = abs(dy)*2;
    int error = 0;
    int y = y0;
    for(int x=x0; x<=x1; x++){
        if(steep){
            setPixel(image, y, x, color);
        }else{
            setPixel(image, x, y, color);
        }
        error += D;
        if(error > dx){
            y += (y1>y0?1:-1);
            error -= dx*2;
        }
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
    line(50, 0, 50, 20, image, red);

    writeTGA(image, "outfile.tga", 0);
    free(image.pixel_bytes);

    return 0;
}

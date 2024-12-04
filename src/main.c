#include "tga.h"
#include <stdio.h>

int main(int argc, char* argv[]){
    
    if(argc <= 1){
        printf("Usage prog [filename]\n");
        return -1;
    }


    struct TGA_image image = loadTGA(argv[1]);
    printHeader(&image.header);

    return 0;
}

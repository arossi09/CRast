#include "tga.h"
#include <stdio.h>

int main(int argc, char* argv[]){
    
    if(argc <= 1){
        printf("Usage prog [filename]\n");
        return -1;
    }

    struct TGA_header header = loadTGA(argv[1]);

    printHeader(&header);

    return 0;
}

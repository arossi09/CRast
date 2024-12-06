#include "obj_loader.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>

struct OBJ_Model loadModel(const char *filename){
    struct OBJ_Model obj;

    int fd = open(filename, O_RDONLY);

    if(fd < 0){
        perror("loadModel: could not open file\n");
        return obj;
    }

    char line[BUFF];
    readLine(fd, line);
    printLine(line);

    close(fd);

    return obj;
}


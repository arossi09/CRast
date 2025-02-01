#include "matrix.h"
#include <stdio.h>

struct Mat3 identityMat3(){
    struct Mat3 mat = {0};
    mat.data[0][0] = 1;
    mat.data[1][1] = 1;
    mat.data[2][2] = 1;
    return mat;
}
struct Mat3 multMat3(struct Mat3 m1, struct Mat3 m2){
    struct Mat3 result = {0};
    for(int row = 0; row < 3; row++){
        for(int col = 0; col < 3; col++){
            result.data[row][col] += m1.data[row][col] * m2.data[col][row];
        }
    }
    return result;
}

void printMat3(struct Mat3 m){
    for(int row = 0; row < 3; row++){
        printf("[");
        for(int col = 0; col < 3; col++){
            printf("%f ", m.data[row][col]);
        }
        printf("]\n");
    }
}

struct Mat4 identityMat4(){
    struct Mat4 mat = {0};
    mat.data[0][0] = 1;
    mat.data[1][1] = 1;
    mat.data[2][2] = 1;
    mat.data[3][3] = 1;
    return mat;
}


struct Mat4 multMat4(struct Mat4 m1, struct Mat4 m2){
    struct Mat4 result = {0};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.data[i][j] += m1.data[i][k] * m2.data[k][j];
            }
        }
    }
    return result;
    return result;
}




struct Mat4 v2m4(float x, float y, float z){
    struct Mat4 result = {0};
    result.data[0][0] = x;
    result.data[1][0] = y;
    result.data[2][0] = z;
    result.data[3][0] = 1.0f;
    return result;
}





#ifndef MATRIX_H
#define MATRIX_H

#define MINIMUM_C 3

//matrix multiplication
//determinate
//transpose

struct Mat4{
    float data[4][4];
};

struct Mat3{
    float data[3][3];
};


struct Mat3 identityMat3();
struct Mat4 identityMat4();
struct Mat3 multMat3(struct Mat3 m1, struct Mat3 m2);
struct Mat4 multMat4(struct Mat4 m1, struct Mat4 m2);
struct Mat4 v2m4(float x, float y, float z);
void printMat3(struct Mat3 m);

#endif

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

ssize_t readLine(int fd, char *buffer);
void printLine(char *line);
void swapi(int *x, int *y);
int max(float x, float y);
int min(float x, float y);

#endif

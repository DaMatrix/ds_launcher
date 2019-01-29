#ifndef TEMPLATE_CONSTANTS_H
#define TEMPLATE_CONSTANTS_H

#include <nds.h>

#include <stdio.h>

const unsigned short SERVER_PORT = 8236;

//some math functions
int min(int a, int b);
int max(int a, int b);
int clamp(int a, int min, int max);

char* format(char* format, ...);

class Console {
    static Console INSTANCE;
};

#endif //TEMPLATE_CONSTANTS_H

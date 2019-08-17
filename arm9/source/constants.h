#ifndef TEMPLATE_CONSTANTS_H
#define TEMPLATE_CONSTANTS_H

#include <nds.h>

#include <cstdarg>
#include <stdio.h>

#include <porklib.h>

using namespace porklib;

const unsigned short SERVER_PORT = 8236;

//some math functions
int min(int a, int b);
int max(int a, int b);
int clamp(int a, int min, int max);

#endif //TEMPLATE_CONSTANTS_H

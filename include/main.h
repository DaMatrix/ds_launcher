#ifndef MAIN_H
#define MAIN_H

#include <misc.h>
#include <netutil.h>
#include <util/buffer.h>

unsigned int decodeHex(char* in);
unsigned int decodeHexOffset(char* in, int offset);
char* encodeHex(int val);

typedef struct {
    char** names;
    int count;
    int current_scroll;
} Namelist;

Namelist parse(char* text);

int roundUp(int numToRound, int multiple);

char* getNameAtIndex(Namelist* list, int i);
char* getCurrentName(Namelist* list);

#endif /* MAIN_H */


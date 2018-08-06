#ifndef BUFFER_H
#define BUFFER_H

#include "misc.h"

typedef struct {
  char* array;
  size_t used;
  size_t size;
} Buffer;

void initArray(Buffer* b, size_t initialSize);

void insertArray(Buffer* b, char element);

void freeArray(Buffer* b);

#endif /* BUFFER_H */


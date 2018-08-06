#include "../include/buffer.h"

void initArray(Buffer* b, size_t initialSize) {
  b->array = (char*) malloc(initialSize * sizeof(char));
  b->used = 0;
  b->size = initialSize;
}

inline void insertArray(Buffer* b, char element) {
  if (b->used == b->size) {
    b->size *= 2;
    b->array = (char*) realloc(b->array, b->size * sizeof(char));
  }
  b->array[b->used++] = element;
}

void freeArray(Buffer* b) {
  free(b->array);
  b->array = NULL;
  b->used = b->size = 0;
}

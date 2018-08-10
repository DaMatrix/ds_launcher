#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <misc.h>

char* readWholeFile(char* name, char* def);
void replaceFile(char* name, char* content);

#endif /* FILEUTIL_H */

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <misc.h>
#include <registry.h>

char* readWholeFile(char* name);
void replaceFile(char* name, char* content);

int getLocalVersion(Entry* entry);

#endif /* FILEUTIL_H */

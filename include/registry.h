#ifndef REGISTRY_H
#define REGISTRY_H

#include <misc.h>
#include <porkmath.h>

static const int MAX_NAME_LENGTH = 28;

typedef struct {
    char* name;
    int version;
} Entry;

typedef struct {
    Entry* entries;
    int count;
    int current_scroll;
} Registry;

Registry parseRegistry(char* text);

Entry* getEntryAtIndex(Registry* registry, int i);
Entry* getCurrentEntry(Registry* registry);

#endif /* REGISTRY_H */


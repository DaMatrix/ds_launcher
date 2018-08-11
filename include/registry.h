#ifndef REGISTRY_H
#define REGISTRY_H

#include <buffer.h>
#include <misc.h>
#include <porkmath.h>

static const int MAX_NAME_LENGTH = 28;

typedef struct {
    char* name;
    char* displayName;
    int version;
    //int id;
    bool batch;
    //char* prefix;
} Entry;

typedef struct {
    Entry* entries;
    int count;
    int current_scroll;
} Registry;

Registry parseRegistry(char* text, bool shouldFree);

Entry* getEntryAtIndex(Registry* registry, int i);
Entry* getCurrentEntry(Registry* registry);

//void tryMarkBatch(Entry* entry, bool batch);

#endif /* REGISTRY_H */


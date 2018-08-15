#include <registry.h>

char* def_prefix = " ";
char* batch_prefix = "#";

Registry parseRegistry(char* text, bool shouldFree) {
    //iprintf("%s\n", text);
    iprintf("Parsing...\n");
    int index = 8;
    Registry registry;
    registry.count = decodeHex(text);
    if (registry.count != 0) {
        registry.entries = (Entry*) malloc(registry.count * sizeof (Entry));
        iprintf("Reading %u names...\n", registry.count);
        for (int i = 0; i < registry.count; i++) {
            int len = decodeHexOffset(text, index);
            index += 8;
            Entry entry;
            entry.name = (char*) malloc((len + 1) * sizeof (char));
            entry.displayName = (char*) malloc((min(len, MAX_NAME_LENGTH) + 1) * sizeof (char));
            for (int j = 0; j < len; j++) {
                if (j < MAX_NAME_LENGTH) {
                    entry.displayName[j] = text[index + j];
                }
                entry.name[j] = text[index + j];
            }
            index += len;
            entry.name[len] = 0;
            entry.displayName[min(len, MAX_NAME_LENGTH)] = 0; //NULL-terminate
            //iprintf("%u (%u bytes) - %s\n", i, len, entry.displayName);
            entry.version = decodeHexOffset(text, index);
            index += 8;
            //entry.id = decodeHexOffset(text, index);
            //index += 8;
            entry.batch = false;
            //entry.prefix = def_prefix;
            registry.entries[i] = entry;
        }
    }
    if (shouldFree) {
        iprintf("Freeing chars...\n");
        free(text);
    }
    iprintf("Done!\n");
    registry.current_scroll = roundUp(1 << 30, registry.count);
    return registry;
}

void tryMarkBatch(Entry* entry, bool batch) {
    if (batch == entry->batch) {
        return;
    }
    if (batch) {
        int localVersion = getLocalVersion(entry);
        if (entry->version > getLocalVersion(entry)) {
            entry->batch = batch;
            //entry->prefix = batch_prefix;
        }
    } else {
        entry->batch = batch;
        //entry->prefix = def_prefix;
    }
}

Entry* getEntryAtIndex(Registry* registry, int i) {
    return &registry->entries[(registry->current_scroll + i) % registry->count];
    //return registry->entries + ((registry->current_scroll + i) % registry->count);
}

Entry* getCurrentEntry(Registry* registry) {
    return &registry->entries[registry->current_scroll % registry->count];
    //return registry->entries + (registry->current_scroll % registry->count);
}

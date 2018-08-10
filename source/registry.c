#include <registry.h>

Registry parseRegistry(char* text) {
    iprintf("Parsing...\n");
    int index = 8;
    Registry registry;
    registry.count = decodeHex(text);
    registry.entries = (Entry*) malloc(registry.count * sizeof(Entry));
    iprintf("Reading %u names...\n", registry.count);
    for (int i = 0; i < registry.count; i++) {
        int len = decodeHexOffset(text, index);
        index += 8;
        Entry entry;
        entry.name = (char*) malloc((min(len, MAX_NAME_LENGTH) + 1) * sizeof (char));
        char* letters = entry.name;
        for (int j = 0; j < len; j++) {
            if (j < MAX_NAME_LENGTH)   {
                letters[j] = text[index + j];
            }
        }
        index += len;
        letters[min(len, MAX_NAME_LENGTH)] = 0; //NULL-terminate
        iprintf("%u (%u bytes) - %s", i, len, letters);
        iprintf("\n");
        entry.version = decodeHexOffset(text, index);
        index += 8;
        registry.entries[i] = entry;
    }
    iprintf("Freeing chars...\n");
    free(text);
    iprintf("Done!\n");
    registry.current_scroll = roundUp(1 << 30, registry.count);
    return registry;
}

Entry* getEntryAtIndex(Registry* registry, int i) {
    return &registry->entries[(registry->current_scroll + i) % registry->count];
}

Entry* getCurrentEntry(Registry* registry)    {
    return &registry->entries[registry->current_scroll % registry->count];
}

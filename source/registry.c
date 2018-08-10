#include <registry.h>

Registry parseRegistry(char* text, bool shouldFree) {
    iprintf("%s\n", text);
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
            iprintf("%u (%u bytes) - %s", i, len, entry.displayName);
            iprintf("\n");
            entry.version = decodeHexOffset(text, index);
            index += 8;
            entry.id = i;
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

void writeCharsToBufAndFree(Buffer* buf, char* c, int len) {
    for (int i = 0; i < len; i++) {
        insertArray(buf, c[i]);
    }
    free(c);
}

void writeCharsToBuf(Buffer* buf, char* c, int len) {
    for (int i = 0; i < len; i++) {
        insertArray(buf, c[i]);
    }
}

char* encodeRegistry(Registry* registry) {
    Buffer buf;
    initArray(&buf, 8);
    writeCharsToBufAndFree(&buf, encodeHex(registry->count), 8);
    for (int i = 0; i < registry->count; i++) {
        writeCharsToBuf(&buf, "00000000", 8); //don't bother encoding the name, as this function is only used for writing to disk
        Entry e = registry->entries[i];
        writeCharsToBufAndFree(&buf, encodeHex(e.version), 8);
    }
    char* temp = (char*) malloc(buf.used * sizeof (char));
    for (int i = 0; i < buf.used; i++) {
        temp[i] = buf.array[i];
    }
    freeArray(&buf);
    return temp;
}

Entry getEntryAtIndex(Registry* registry, int i) {
    return registry->entries[(registry->current_scroll + i) % registry->count];
    //return registry->entries + ((registry->current_scroll + i) % registry->count);
}

Entry getCurrentEntry(Registry* registry) {
    return registry->entries[registry->current_scroll % registry->count];
    //return registry->entries + (registry->current_scroll % registry->count);
}

void addEntry(Registry* registry, Entry* e) {
    registry->count++;
    registry->entries = (Entry*) realloc(registry->entries, registry->count * sizeof (Entry));
    Entry internal = registry->entries[registry->count - 1];
    internal.displayName = e->displayName;
    internal.name = e->name;
    internal.version = e->version;
    internal.id = e->id;
}

void removeEntry(Registry* registry, Entry* e) {
    Entry* old = registry->entries;
    Entry* new = (Entry*) malloc((--registry->count) * sizeof (Entry));
    int j = 0;
    for (int i = 0; i <= registry->count; i++) {
        if (old[i].id != e->id) {
            new[j++] = old[i];
        }
    }
    registry->entries = new;
    free(old);
}

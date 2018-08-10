#include <main.h>

char* hex = "0123456789abcdef";

void insert(Buffer* b, char* c) {
    int len = sizeof (c) / sizeof (c[0]);
    for (int i = 0; i < len; i++) {
        insertArray(&b, c[i]);
    }
}

int main(void) {
    consoleDemoInit(); //setup the sub screen for printing
    /*
        iprintf("\n\n\tStarting the PorkStore...\n\n");
        initNetworking();

        iprintf("Initializing filesystem...\n");
        fatInitDefault();

        Namelist list = parse(requestData("/index.txt"));*/
    Namelist list = parse("70000000b0000000httpget.nds01000000LEGO Battles.nds91000000Mario Kart DS (NoSSL).nds81000000New Super Mario Bros.nds81000000New Super Mario Bros.savc2000000Pokemon - Black Version (daporkchop.net).ndse0000000WfcPatcher.exe");

    int mode = -1;

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        u16 keys = keysDown();
        if (mode == -1)  {
            mode = 0;
            goto DRAW_MENU;
        } else if (mode == 0) {
            if ((keys & KEY_UP)) {
                list.current_scroll--;
                goto DRAW_MENU;
            }
            if ((keys & KEY_DOWN)) {
                list.current_scroll++;
                goto DRAW_MENU;
            }
            if ((keys & KEY_A)) {
                mode = 1;
                goto DRAW_ITEM;
            }
        } else if (mode == 1) {
            if ((keys & KEY_X)) {
                //TODO: download
            }
            if ((keys & KEY_B)) {
                mode = 0;
                goto DRAW_MENU;
            }
        }

        if (false) {
DRAW_MENU:
            while (0) {
            }
            //excuse this mess please, but it goes faster than a loop
            iprintf("Press X to check for updates\n\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n> %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n\n\n",
                    getNameAtIndex(&list, -9),
                    getNameAtIndex(&list, -8),
                    getNameAtIndex(&list, -7),
                    getNameAtIndex(&list, -6),
                    getNameAtIndex(&list, -5),
                    getNameAtIndex(&list, -4),
                    getNameAtIndex(&list, -3),
                    getNameAtIndex(&list, -2),
                    getNameAtIndex(&list, -1),
                    getNameAtIndex(&list, 0),
                    getNameAtIndex(&list, 1),
                    getNameAtIndex(&list, 2),
                    getNameAtIndex(&list, 3),
                    getNameAtIndex(&list, 4),
                    getNameAtIndex(&list, 5),
                    getNameAtIndex(&list, 6),
                    getNameAtIndex(&list, 7),
                    getNameAtIndex(&list, 8),
                    getNameAtIndex(&list, 9));
            /*iprintf("Press X to check for updates\n\n");
            for (int i = -10; i < 8; i++) {
                iprintf(i == 0 ? "> %s\n" : "  %s\n", list.names[(scroll + i) % list.count]);
            }*/
        }
        if (false) {
DRAW_ITEM:
            while (0) {
            }
            char* name = getCurrentName(&list);
            int ver = decodeHex("0000000f");
            iprintf("\n%s\n\nLatest version:    %u\nInstalled version: %u\n\nPress X to download\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", name, ver, 0);
        }
    }

    return 0;
}

unsigned int decodeHex(char* in) {
    unsigned int val = 0;
    for (int i = 0; i < 8; i++) {
        char c = in[i];
        for (int j = 0; j < 16; j++) {
            if (hex[j] == c) {
                val |= j << (i << 2);
                break;
            }
        }
    }
    return val;
}

unsigned int decodeHexOffset(char* in, int offset) {
    unsigned int val = 0;
    for (int i = 0; i < 8; i++) {
        char c = in[i + offset];
        for (int j = 0; j < 16; j++) {
            if (hex[j] == c) {
                val |= j << (i << 2);
                break;
            }
        }
    }
    return val;
}

char* encodeHex(int val) {
    char* letters[8];
    for (int i = 0; i < 8; i++) {
        letters[i] = hex[(val >> (i << 2)) & 0xF];
    }
    return letters;
}

Namelist parse(char* text) {
    iprintf("Parsing...\n");
    int index = 8;
    Namelist list;
    list.count = decodeHex(text);
    list.names = (char**) malloc(list.count * sizeof (char*));
    iprintf("Reading %u names...\n", list.count);
    for (int i = 0; i < list.count; i++) {
        int len = decodeHexOffset(text, index);
        index += 8;
        list.names[i] = (char*) malloc((len + 1) * sizeof (char));
        char* letters = list.names[i];
        for (int j = 0; j < len; j++) {
            letters[j] = text[index++];
        }
        letters[len] = 0; //NULL-terminate
        iprintf("%u (%u bytes) - %s", i, len, letters);
        iprintf("\n");
    }
    iprintf("Freeing chars...\n");
    free(text);
    iprintf("Done!\n");
    list.current_scroll = roundUp(1 << 30, list.count);
    return list;
}

int roundUp(int numToRound, int multiple) {
    if (multiple == 0) {
        return numToRound;
    }
    int remainder = numToRound % multiple;
    if (remainder == 0) {
        return numToRound;
    }
    return numToRound + multiple - remainder;
}

char* getNameAtIndex(Namelist* list, int i) {
    return list->names[(list->current_scroll + i) % list->count];
}

char* getCurrentName(Namelist* list)    {
    return list->names[list->current_scroll % list->count];
}

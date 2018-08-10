#include <main.h>

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
    Registry registry = parseRegistry("70000000b0000000httpget.nds1000000001000000LEGO Battles.nds1000000091000000Mario Kart DS (NoSSL).nds1000000081000000New Super Mario Bros.nds1000000081000000New Super Mario Bros.sav10000000c2000000Pokemon - Black Version (daporkchop.net).nds10000000e0000000WfcPatcher.exe10000000");

    int mode = -1;

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        u16 keys = keysHeld();
        if (mode == -1) {
            mode = 0;
            goto DRAW_MENU;
        } else if (mode == 0) {
            if ((keys & KEY_UP)) {
                registry.current_scroll--;
                goto DRAW_MENU;
            }
            if ((keys & KEY_DOWN)) {
                registry.current_scroll++;
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
                    getEntryAtIndex(&registry, -9)->name,
                    getEntryAtIndex(&registry, -8)->name,
                    getEntryAtIndex(&registry, -7)->name,
                    getEntryAtIndex(&registry, -6)->name,
                    getEntryAtIndex(&registry, -5)->name,
                    getEntryAtIndex(&registry, -4)->name,
                    getEntryAtIndex(&registry, -3)->name,
                    getEntryAtIndex(&registry, -2)->name,
                    getEntryAtIndex(&registry, -1)->name,
                    getEntryAtIndex(&registry, 0)->name,
                    getEntryAtIndex(&registry, 1)->name,
                    getEntryAtIndex(&registry, 2)->name,
                    getEntryAtIndex(&registry, 3)->name,
                    getEntryAtIndex(&registry, 4)->name,
                    getEntryAtIndex(&registry, 5)->name,
                    getEntryAtIndex(&registry, 6)->name,
                    getEntryAtIndex(&registry, 7)->name,
                    getEntryAtIndex(&registry, 8)->name,
                    getEntryAtIndex(&registry, 9)->name);
            goto WAIT;
        }
        if (false) {
DRAW_ITEM:
            while (0) {
            }
            Entry* entry = getCurrentEntry(&registry);
            iprintf("\n%s\n\nLatest version:    %u\nInstalled version: %u\n\nPress X to download\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", entry->name, entry->version, 0);
        }
        if (false) {
WAIT:
            for (int i = 0; i < 10; i++) {
                swiWaitForVBlank();
            }
        }
    }

    return 0;
}

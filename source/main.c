#include <main.h>

int main(void) {
    if (fatInitDefault()) {
        DIR *pdir;
        struct dirent *pent;

        pdir = opendir("/");

        if (pdir) {

            while ((pent = readdir(pdir)) != NULL) {
                if (strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)
                    continue;
                if (pent->d_type == DT_DIR)
                    iprintf("[%s]\n", pent->d_name);
                else
                    iprintf("%s\n", pent->d_name);
            }
            closedir(pdir);
        } else {
            iprintf("opendir() failure; terminating\n");
        }
    } else {
        swiWaitForVBlank();
        swiWaitForVBlank();
        swiWaitForVBlank();
        iprintf("fatInitDefault failure: terminating\n");
        swiWaitForVBlank();
        swiWaitForVBlank();
        swiWaitForVBlank();
        return 0;
    }

    consoleDemoInit(); //setup the sub screen for printing
    iprintf("\n\n\tStarting the PorkStore...\n\n");
    initNetworking();

    //iprintf("%s\n", requestData("/a"));
    //iprintf("%s\n", requestData("/b"));

    Registry registry = parseRegistry(requestData("/index.txt"), true);
    //Registry registry = parseRegistry("20000000f0000000DaPorkchop_.png20000000fbf2909080000000logo.bmp300000002ce58c80", false);

    Registry local = parseRegistry(readWholeFile("/porkstore_index.txt", "00000000"), true);
    //replaceFile("/porkstore_index.txt", "00000000\nYou've been JEFFED");

    /*if (1) {
        iprintf("%u\n", local.count);
        for (int i = 0; i < local.count; i++) {
            Entry entry = local.entries[i];
            iprintf("%u - %s (v%u)\n", i, entry->displayName, entry.version);
        }

        while (1) {
            swiWaitForVBlank();
            int keys = keysDown();
            if (keys & KEY_START) break;
        }
        return 0;
    }*/

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
            if ((keys % KEY_X)) {
                goto CHECK_UPDATES;
            }
        } else if (mode == 1) {
            if ((keys & KEY_X)) {
                Entry* upstream = getCurrentEntry(&registry);
                int localVersion = decodeHex(readWholeFile(encodeHexTerm(entry->id)));
CHECK:
                if (localVersion != 0) {
                    //game is already owned
                    if (upstream->version > localVersion) {
                        current->version = upstream->version;
                    } else {
                        iprintf("\n\n\n\n\n\n\n\n\n\n\nGame already installed,\nno updates found!\n\nPress B to continue\n\n\n\n\n\n\n\n\n");
                        mode = 2;
                        goto WAIT;
                    }
                } else {
                    iprintf("Adding entry to registry\n");
                    addEntry(&local, upstream);
                }
                downloadFile(upstream);

                iprintf("Encoding registry to text\n");
                char* encoded = encodeRegistry(&local);
                iprintf("Writing registry to disk\n");
                iprintf("%s\n", encoded);
                replaceFile("/porkstore_index.txt", encoded);
                iprintf("Written!\n");
                free(encoded);
                iprintf("Done!\n");
                mode = -1;
                //goto DRAW_MENU;
            }
            if ((keys & KEY_B)) {
                mode = -1;
                //goto DRAW_MENU;
            }
        } else if (mode == 2) {
            if ((keys & KEY_B)) {
                mode = -1;
                //goto DRAW_MENU;
            }
        }

        if (false) {
DRAW_MENU:
            while (0) {
            }
            //excuse this mess please, but it goes faster than a loop
            iprintf("Press X to check for updates\n\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n> %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n  %s\n\n\n",
                    getEntryAtIndex(&registry, -9)->displayName,
                    getEntryAtIndex(&registry, -8)->displayName,
                    getEntryAtIndex(&registry, -7)->displayName,
                    getEntryAtIndex(&registry, -6)->displayName,
                    getEntryAtIndex(&registry, -5)->displayName,
                    getEntryAtIndex(&registry, -4)->displayName,
                    getEntryAtIndex(&registry, -3)->displayName,
                    getEntryAtIndex(&registry, -2)->displayName,
                    getEntryAtIndex(&registry, -1)->displayName,
                    getEntryAtIndex(&registry, 0)->displayName,
                    getEntryAtIndex(&registry, 1)->displayName,
                    getEntryAtIndex(&registry, 2)->displayName,
                    getEntryAtIndex(&registry, 3)->displayName,
                    getEntryAtIndex(&registry, 4)->displayName,
                    getEntryAtIndex(&registry, 5)->displayName,
                    getEntryAtIndex(&registry, 6)->displayName,
                    getEntryAtIndex(&registry, 7)->displayName,
                    getEntryAtIndex(&registry, 8)->displayName,
                    getEntryAtIndex(&registry, 9)->displayName);
            goto WAIT;
        }
        if (false) {
DRAW_ITEM:
            while (0) {
            }
            Entry* entry = getCurrentEntry(&registry);
            int localVersion = decodeHex(readWholeFile(encodeHexTerm(entry->id)));
            iprintf("\n%s\n\nLatest version:    %u\nInstalled version: %u\n\nPress X to download\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", entry->displayName, entry->version, localVersion);
        }
        if (false) {
CHECK_UPDATES:
            while (0) {
            }
            downloadFile("/download/DaPorkchop_.png");
        }
        if (false) {
WAIT:
            for (int i = 0; i < 5; i++) {
                swiWaitForVBlank();
            }
        }
    }

    return 0;
}

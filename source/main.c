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

    Registry registry = parseRegistry(requestIndex(), true);

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
            if ((keys & KEY_L)) {
                registry.current_scroll -= 5;
                goto DRAW_MENU;
            }
            if ((keys & KEY_R)) {
                registry.current_scroll += 5;
                goto DRAW_MENU;
            }
            if ((keys & KEY_A)) {
                mode = 1;
                goto DRAW_ITEM;
            }
            if ((keys & KEY_X)) {
                goto CHECK_UPDATES;
            }
            if ((keys & KEY_SELECT)) {
                Entry* entry = getCurrentEntry(&registry);
                entry->batch = true;
                goto DRAW_MENU;
            }
            if ((keys & KEY_START)) {
                Entry* entry = getCurrentEntry(&registry);
                entry->batch = false;
                goto DRAW_MENU;
            }
            if ((keys & KEY_Y)) {
                //clear current registry
                for (int i = 0; i < registry.count; i++) {
                    Entry* entry = &registry.entries[i];
                    free(entry->displayName);
                    free(entry->name);
                    //entry->id = 0;
                    entry->version = 0;
                }
                free(registry.entries);
                registry.count = 0;
                registry = parseRegistry(requestIndex(), true);
                goto DRAW_MENU;
            }
        } else if (mode == 1) {
            if ((keys & KEY_X)) {
                Entry* upstream = getCurrentEntry(&registry);
                int localVersion = getLocalVersion(upstream);
                int remoteVersion = upstream->version;
                if (localVersion != 0) {
                    //game is already owned
                    if (remoteVersion > localVersion) {
                        localVersion = remoteVersion;
                    } else {
                        iprintf("\n\n\n\n\n\n\n\n\n\n\nGame already installed,\nno updates found!\n\nPress B to continue\n\n\n\n\n\n\n\n\n");
                        mode = 2;
                        goto WAIT;
                    }
                }
                downloadFile(upstream);

                iprintf("Done!\n");
                mode = 0;
                goto DRAW_MENU;
            }
            if ((keys & KEY_B)) {
                mode = 0;
                goto DRAW_MENU;
            }
            if ((keys & KEY_Y)) {
                mode = 3;
                goto DRAW_CONFIRM_REMOVE;
            }
        } else if (mode == 2) {
            if ((keys & KEY_B)) {
                mode = 0;
                goto DRAW_MENU;
            }
        } else if (mode == 3) {
            if ((keys & KEY_B)) {
                mode = 1;
                goto DRAW_ITEM;
            }
            if ((keys & KEY_A)) {
                Entry* entry = getCurrentEntry(&registry);
                remove(entry->name);
                int len = strlen(entry->name);
                char* sav = (char*) malloc((len + 1) * sizeof(char));
                for (int i = len - 1; i >= 0; i--)  {
                    sav[i] = entry->name[i];
                }
                sav[len] = 0;
                sav[len - 3] = 's';
                sav[len - 2] = 'a';
                sav[len - 1] = 'v';
                remove(sav);
                free(sav);
                mode = 2;
                goto DRAW_REMOVED;
            }
        }

        if (false) {
DRAW_MENU:
            while (0) {
            }
            //excuse this mess please, but it goes faster than a loop
            iprintf("X to run updates+batch\nY to refresh list\nL/R to scroll fast\nSEL to add batch item\nSTART to remove batch item\n\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n>%s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n\n\n",
                    getEntryAtIndex(&registry, -7)->batch ? "#" : " ", getEntryAtIndex(&registry, -7)->displayName,
                    getEntryAtIndex(&registry, -6)->batch ? "#" : " ", getEntryAtIndex(&registry, -6)->displayName,
                    getEntryAtIndex(&registry, -5)->batch ? "#" : " ", getEntryAtIndex(&registry, -5)->displayName,
                    getEntryAtIndex(&registry, -4)->batch ? "#" : " ", getEntryAtIndex(&registry, -4)->displayName,
                    getEntryAtIndex(&registry, -3)->batch ? "#" : " ", getEntryAtIndex(&registry, -3)->displayName,
                    getEntryAtIndex(&registry, -2)->batch ? "#" : " ", getEntryAtIndex(&registry, -2)->displayName,
                    getEntryAtIndex(&registry, -1)->batch ? "#" : " ", getEntryAtIndex(&registry, -1)->displayName,
                    getEntryAtIndex(&registry, 0)->batch ? "#" : " ", getEntryAtIndex(&registry, 0)->displayName,
                    getEntryAtIndex(&registry, 1)->batch ? "#" : " ", getEntryAtIndex(&registry, 1)->displayName,
                    getEntryAtIndex(&registry, 2)->batch ? "#" : " ", getEntryAtIndex(&registry, 2)->displayName,
                    getEntryAtIndex(&registry, 3)->batch ? "#" : " ", getEntryAtIndex(&registry, 3)->displayName,
                    getEntryAtIndex(&registry, 4)->batch ? "#" : " ", getEntryAtIndex(&registry, 4)->displayName,
                    getEntryAtIndex(&registry, 5)->batch ? "#" : " ", getEntryAtIndex(&registry, 5)->displayName,
                    getEntryAtIndex(&registry, 6)->batch ? "#" : " ", getEntryAtIndex(&registry, 6)->displayName,
                    getEntryAtIndex(&registry, 7)->batch ? "#" : " ", getEntryAtIndex(&registry, 7)->displayName);
            goto WAIT;
        }
        if (false) {
DRAW_ITEM:
            while (0) {
            }
            Entry* entry = getCurrentEntry(&registry);
            int localVersion = getLocalVersion(entry);
            iprintf("Name:\n%s\n\nLatest version:    %u\nInstalled version: %u\n\nPress X to download\nPress Y to uninstall\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", entry->displayName, entry->version, localVersion);
        }
        if (false) {
DRAW_CONFIRM_REMOVE:
            while (0) {
            }
            iprintf("Are you sure?\n\nPress A to delete\nPress B to cancel\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        }
        if (false) {
DRAW_REMOVED:
            while (0) {
            }
            iprintf("Removed.\n\n\nPress B to continue\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        }
        if (false) {
CHECK_UPDATES:
            while (0) {
            }
            for (registry.current_scroll = 0; registry.current_scroll < registry.count; registry.current_scroll++) {
                Entry* upstream = getCurrentEntry(&registry);
                int localVersion = getLocalVersion(upstream);
                int remoteVersion = upstream->version;
                if (upstream->batch) {
                    upstream->batch = false;
                } else if (localVersion != 0) {
                    //game is already owned
                    if (remoteVersion > localVersion) {
                        localVersion = remoteVersion;
                    } else {
                        goto DONE;
                    }
                } else {
                    goto DONE;
                }
                downloadFile(upstream);
DONE:
                iprintf("Done!\n");
            }
            registry.current_scroll = roundUp(1 << 30, registry.count);
            for (int i = 0; i < 300; i++) {
                swiWaitForVBlank();
            }
            iprintf("\n\n\n\n\n\n\n\n\n\n\n\nUpdating finished!\n\nPress B to continue\n\n\n\n\n\n\n\n\n");
            mode = 2;
            goto WAIT;
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

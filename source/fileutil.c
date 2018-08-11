#include <fileutil.h>

char* readWholeFile(char* name) {
    char* aName = (char*) malloc(17 + strlen(name));
    sprintf(aName, "/porkstore_index/%s", name);
    free(name);
    if (access(aName, R_OK) != -1) {
        FILE* fp = fopen(aName, "r");
        free(aName);
        fseek(fp, 0L, SEEK_END);
        int len = ftell(fp) + 1;
        rewind(fp);
        char* content = (char*) malloc(len * sizeof (char));
        fgets(content, len, fp);
        fclose(fp);
        return content;
    } else {
        free(aName);
        return "00000000";
    }
}

void replaceFile(char* name, char* content) {
    char* tempName = (char*) malloc((strlen(name) + 17) * sizeof (char));
    sprintf(tempName, "/porkstore_index/%s", name);
    free(name);

    FILE* fp = fopen(name, "w");
    fprintf(fp, "%s", content);
    fclose(fp);

    free(tempName);
    //remove(name);
    //rename(tempName, name);
    //free(tempName);
}

int getLocalVersion(Entry* entry) {
    char* name = (char*) malloc((strlen(entry->name) + 1) * sizeof (char));
    sprintf(name, "/%s", entry->name);
    if (access(name, R_OK) != -1) {
        FILE* fp = fopen(name, "rb");
        free(name);
        name = (char*) malloc(9 * sizeof (char));
        fseek(fp, -8, SEEK_END);
        fread(name, sizeof(char), 8, fp);
        fclose(fp);
        name[8] = 0;
        int ver = decodeHex(name);
        free(name);
        return ver;
    } else {
        free(name);
        return 0;
    }
}

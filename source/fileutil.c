#include <fileutil.h>

char* readWholeFile(char* name, char* def) {
    if (access(name, R_OK) != -1) {
        FILE* fp = fopen(name, "r");
        if (fp == NULL) {
            //this fixes calling free() on the output of this function if def is a hardcoded string
            char* hotfix = (char*) malloc(strlen(def) * sizeof(char));
            sprintf(hotfix, "%s", def);
            return hotfix;
        }
        fseek(fp, 0L, SEEK_END);
        int len = ftell(fp) + 1;
        rewind(fp);
        char* content = (char*) malloc(len * sizeof (char));
        fgets(content, len, fp);
        fclose(fp);
        return content;
    } else {
        return def;
    }
}

void replaceFile(char* name, char* content) {
    char* tempName = (char*) malloc((strlen(name) + 5) * sizeof(char));
    sprintf(tempName, "%s.temp", name);
    
    FILE* fp = fopen(tempName, "w");
    fputs(content, fp);
    fclose(fp);
    
    remove(name);
    rename(tempName, name);
    free(tempName);
}

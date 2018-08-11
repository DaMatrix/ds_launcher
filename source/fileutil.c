#include <fileutil.h>

char* readWholeFile(char* name, char* def) {
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
    char* tempName = (char*) malloc((strlen(name) + 17) * sizeof(char));
    sprintf(tempName, "/porkstore_index/%s", name);
    free(name);

    FILE* fp = fopen(name, "w");
    free(tempName);
    fprintf(fp, "%s", content);
    fclose(fp);

    //remove(name);
    //rename(tempName, name);
    //free(tempName);
}

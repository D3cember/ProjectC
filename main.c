#include <stdio.h>
#include <stdlib.h>
#include "preproc.h"

int main() {
    const char* amFile = "/Users/december/Desktop/C Class/ProjectC/ProjectC/input.asm"; // Use the actual absolute path
    const char* asFile = "/Users/december/Desktop/C Class/ProjectC/ProjectC/output.asm"; // Use the actual absolute path

    FILE *as = fopen(amFile, "r");
    if (!as) {
        perror("Error opening input.asm");
        return EXIT_FAILURE;
    }

    FILE *am = fopen(asFile, "w");
    if (!am) {
        perror("Error opening output.asm");
        fclose(as);
        return EXIT_FAILURE;
    }

    process_file(as, am);

    fclose(as);
    fclose(am);

    return 0;
}

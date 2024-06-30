#include "preproc.h"
#include "errors.h"
#include "util.h"
#include "data_struct.h"
#include <stdio.h>

int main() {
    FILE *inputFile = fopen("input.asm", "r");
    if (!inputFile) {
        perror("Failed to open file");
        return 1;
    }

    fpos_t position;
    int lineCount = 0;

    if (fgetpos(inputFile, &position) != 0) {
        perror("Failed to get file position");
        return 1;
    }

    char *macroContent = save_macr_content(inputFile, &position, &lineCount);

    if (macroContent != NULL) {
        printf("Macro content: %s\n", macroContent);
        free(macroContent); // Assuming copy_contain allocates memory for the macro content
    } else {
        printf("Failed to save macro content\n");
    }

    fclose(inputFile);
    return 0;
}

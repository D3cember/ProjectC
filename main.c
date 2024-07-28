#include <stdio.h>
#include <stdlib.h>
#include "../headers/preproc.h"
#include "../headers/data_struct.h"
#include "../headers/globaldefine.h"

int main() {
    FILE *input_file = fopen("input.asm", "r");
    FILE *output_file = fopen("output.asm", "w");
    if (!input_file || !output_file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    process_file(input_file, output_file);

    fclose(input_file);
    fclose(output_file);

    return 0;
}

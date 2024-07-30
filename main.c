#include "headers/preproc.h"
#include "headers/data_struct.h"
#include "headers/errors.h"
#include "headers/globaldefine.h"
#include "headers/util.h"


int main(void) {
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

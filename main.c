#include "headers/data_struct.h"
#include "headers/preproc.h"
#include "headers/errors.h"
#include "headers/first_pass.h"
#include "headers/globaldefine.h"
#include "headers/util.h"


int main(void) {
    const char *input_filename = "input.asm";
    const char *output_filename = "output.asm";
    const char *symbol_table_filename = "symbol_table.txt";
    
    FILE *input_file = fopen(input_filename, "r");
    FILE *output_file = fopen(output_filename, "w");
    if (!input_file || !output_file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    process_file(input_file, output_file);
    fclose(input_file);
    fclose(output_file);

    first_pass(output_filename);

    print_symbol_table(symbol_table_filename);

    free_symbol_table();

    return 0;
}

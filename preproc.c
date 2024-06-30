
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_struct.h"
#include "errors.h"
#include "util.h"
#include "preproc.h"

#define MAX_LINE_LENGTH 80

char *save_macr_content(FILE *as,fpos_t *fpos, int *line_count){
    int macro_length;
    char *macro;
    char line[MAX_LINE_LENGTH];
    if (fsetpos(as,fpos) != 0){
        print_internal_error(ERROR_CODE_2);
        return NULL;
    }
    macro_length =0;
    line[0] = '\0';
    while ((fgets(line, MAX_LINE_LENGTH, as)) && !feof(as)) {
        if (strcmp(line, "endmacr\n") != 0 && strstr(line, "endmacr") != NULL) {
            print_internal_error(ERROR_CODE_3);
            return NULL;
        }
        (*line_count)++;
        if (strcmp(line, "endmacr\n") != 0) {
            macro_length += strlen(line);
        }
    }

    macro = copy_contain(as,fpos,macro_length);
    return macro;
}

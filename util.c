
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include "data_struct.h"
#include "stdio.h"
#include "util.h"
#include "errors.h"
#include "preproc.h"
#include "globaldefine.h"



char *copy_contain(FILE *as, fpos_t *fpos, int length) {
    char *string;

    if (fsetpos(as, fpos) != 0) {
        print_internal_error(ERROR_CODE_2);
        return NULL;
    }

    string = handle_malloc((length + 1) * sizeof(char));
    if (!string) {
        print_internal_error(ERROR_CODE_1);
        return NULL;
    }

    fread(string, sizeof(char), length, as);
    string[length] = '\0';

    return string;
}

char* trim_whitespace(char* str) {
    char* end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    end[1] = '\0';

    return str;
}

const char *reserved_keywords[MAX_KEYWORDS] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop",".data",".string",".entry",".extern"};




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "headers/util.h"
#include "headers/errors.h"


const char *reserved_keywords[MAX_KEYWORDS] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop",".data",".string",".entry",".extern","r0","r1","r2","r3","r4","r5","r6","r7","macr","endmacr"};
char* Macro_name(const char *line) {
    const char *macr = "macr";
    const char *ptr = line;
    char *identifier = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
    char *id_ptr = identifier;

    /*Skip leading whitespaces*/ 
    while (isspace(*ptr)) {
        ptr++;
    }

    /*Check if "macr" is present*/ 
    if (strncmp(ptr, macr, strlen(macr)) == 0) {
        ptr += strlen(macr);
    }

    /*Skip whitespaces after "macr"*/ 
    while (isspace(*ptr)) {
        ptr++;
    }

    /*Copy the identifier*/ 
    while (*ptr != '\n' && !isspace(*ptr)) {
        *id_ptr++ = *ptr++;
    }
    *id_ptr = '\0';
    

    return identifier;
}

int is_macro_call(const char *line) {
    const char *start = line;
    const char *end;
    const char *space;
    const char *tab;

    /*Remove leading and trailing whitespace*/ 
    while (isspace(*start)) {
        start++;
    }

    end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) {
        end--;
    }

    /*Check if the line contains a single word*/ 
    space = strchr(start, ' ');
    tab = strchr(start, '\t');
    return (space == NULL && tab == NULL);
}
int check_macro_declaration_format(const char *line) {
    const char *macr = "macr";
    const char *ptr = line;
    
    /*Skip leading whitespaces*/
    while (isspace(*ptr)) {
        ptr++;
    }

    /* Check if "macr" is present at the beginning */
    if (strncmp(ptr, macr, strlen(macr)) != 0) {
        return 0;
    }
    ptr += strlen(macr);

    /* Skip whitespaces after "macr" */
    while (isspace(*ptr)) {
        ptr++;
    }

    /* Check if macro name is valid */
    if (!*ptr || !isalpha((unsigned char)*ptr)) {
        return 0;
    }

    /* Move ptr to the end of the macro name */
    while (*ptr && (isalnum((unsigned char)*ptr) || *ptr == '_')) {
        ptr++;
    }

    /* Ensure no extra non-whitespace characters after the macro name */
    while (*ptr) {
        if (!isspace(*ptr)) {
            return 0;
        }
        ptr++;
    }

    return 1;
}


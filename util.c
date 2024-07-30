#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "headers/util.h"
#include "headers/errors.h"



char* trim_whitespace(char* str) {
    char* end;

    /*Trim leading space*/ 
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  /*All spaces?*/ 
        return str;

    /*Trim trailing space*/ 
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    /*Write new null terminator*/ 
    *(end + 1) = 0;

    return str;
}

const char *reserved_keywords[MAX_KEYWORDS] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop",".data",".string",".entry",".extern"};
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



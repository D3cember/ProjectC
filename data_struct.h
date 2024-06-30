#ifndef DATA_STRCT_H
#define DATA_STRCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *handle_malloc(size_t size);

struct Macro {
    char macroName[200];
    char *macroContent;
    struct Macro *nextMacro;
};

typedef struct {
    char *file_name;
    int line;
    int column;
} location;

struct Macro *newMacro(const char *macroName, const char *macroContent);

#endif

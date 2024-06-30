#ifndef DATA_STRCT_H
#define DATA_STRCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototype for handle_malloc
void *handle_malloc(size_t size);

// Define the Macro struct
struct Macro {
    char macroName[200];
    char *macroContent;
    struct Macro *nextMacro;
};

// Function prototype for creating a new Macro
struct Macro *newMacro(const char *macroName, const char *macroContent, int line_num);

#endif // DATA_STRCT_H

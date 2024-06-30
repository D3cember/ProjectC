//
// Created by רן לוגסי on 30/06/2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_struct.h"
#include "errors.h"

void *handle_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

struct Macro *newMacro(const char *macroName, const char *macroContent, int line_num){
    struct Macro *temp =(struct Macro*) handle_malloc(sizeof(struct Macro));
    strncpy(temp->macroName,macroName, sizeof(temp->macroName)-1);
    temp->macroContent= strdup(macroContent);
    if (temp->macroContent == NULL){
        print_internal_error(ERROR_CODE_1);
        free(temp);
    }
    temp->newMacro == NULL;
    return temp;
}




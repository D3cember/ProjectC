#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_struct.h"
#include "preproc.h"
#include "globaldefine.h"



void process_file(FILE *as, FILE *am) {
    if (!as || !am) {
        print_internal_error(ERROR_CODE_4);
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    char in_macro = 0;
    LinkedListOfMacro* macroTable = (LinkedListOfMacro*)handle_malloc(sizeof(LinkedListOfMacro));
    if (!macroTable) {
        print_internal_error(ERROR_CODE_1);
        exit(EXIT_FAILURE);
    }
    macroTable->head=NULL;
    macroTable->tail=NULL;
    NodeOnList* currentMacro = NULL;
    while (fgets(line, MAX_LINE_LENGTH, as)!= 0) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        if (strstr(line, "endmacr") != NULL) { // if fount stop of declare.
            if (in_macro) {
                in_macro = 0;
                currentMacro = NULL; // End of macro, reset currentMacro
            }
        } else if (in_macro) { // if in macro declareation store the content.
            if (currentMacro != NULL) {
                add_macro_content(currentMacro, line);
            }
        } else if (strstr(line, "macr") != NULL) {
            addNode(macroTable, line);
            in_macro = 1;
            currentMacro = macroTable->tail; // Set currentMacro to the newly added node
        }
        fputs(line, am);
    }
    free_linked_list(macroTable);
    free(macroTable);
}


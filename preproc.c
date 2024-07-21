#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_struct.h"
#include "preproc.h"
#include "globaldefine.h"

// --------- TEST-------------- //
int check_endmacr_format(const char *line) {
    const char *ptr = strstr(line, "endmacr");
    if (!ptr) {
        return 0;
    }
    ptr += strlen("endmacr");
    while (*ptr) {
        if (!isspace((unsigned char)*ptr)) {
            return 0;
        }
        ptr++;
    }
    return 1;
}

NodeOnList* find_macro(LinkedListOfMacro *macroTable, const char *name) {
    if (!macroTable || !name) return NULL;
    NodeOnList *current = macroTable->head;
    while (current != macroTable->tail){
        if (current->name && strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
// --------- TEST-------------- //

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
        if (strstr(line, "endmacr") != NULL) { // CASE 1: End of macro decleration
            if (in_macro) { // if inside of macro decleration and arrive "endmacr" line.
                if (!check_endmacr_format(line)) {
                    print_internal_error(ERROR_CODE_5);
                    exit(EXIT_FAILURE);
                }
                in_macro = 0;
                currentMacro = NULL; // End of macro, reset currentMacro
            }
        } else if (in_macro) { // Case 2: Inside Macro Declaration
            if (currentMacro != NULL) {
                add_macro_content(currentMacro, line);
            }
        } else if (strstr(line, "macr") != NULL) {// Case 3: Start of Macro Declaration
            addNode(macroTable, line);
            in_macro = 1;
            currentMacro = macroTable->tail; // Set currentMacro to the newly added node
        } else { // Case 4: Check for Macro Call or Normal Line
            NodeOnList *calledMacro = find_macro(macroTable, line);
            if (calledMacro) { // If it's a macro call
                NodeOfMacroContentList *content = calledMacro->Macro_content->head;
                while (content) {
                    fputs(content->line, am);
                    content = content->next;
                }
            } else { // If it's a normal line
                fputs(line, am);
            }
        }
    }

    free_linked_list(macroTable);
    free(macroTable);
}


//TODO : 1) check if in the end of decleration "endmacr" there is not extra text ( Done).
//       2) create method to replace the text when call for macro.
//       3) check there is no calls for non existent macro.
//       4) check that macro name isnt start


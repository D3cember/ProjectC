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
    while (current) {
        if (current->name && strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void process_file(FILE *as, FILE *am) {
    if (!as || !am) {
        print_internal_error(ERROR_CODE_4);
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    char in_macro = 0;
    LinkedListOfMacro *macroTable = (LinkedListOfMacro *) handle_malloc(sizeof(LinkedListOfMacro));
    if (!macroTable) {
        print_internal_error(ERROR_CODE_1);
        exit(EXIT_FAILURE);
    }
    macroTable->head = NULL;
    macroTable->tail = NULL;
    NodeOnList *currentMacro = NULL;
    while (fgets(line, MAX_LINE_LENGTH, as) != 0) {
        char *identifier = Macro_name(line);
// --------- CASE 1: End of macro decleration ---------- //
        if (strstr(line, "endmacr") != NULL) { // CASE 1: End of macro decleration
            if (in_macro) { // if inside of macro decleration and arrive "endmacr" line.
                if (!check_endmacr_format(line)) {
                    print_internal_error(ERROR_CODE_5);
                    exit(EXIT_FAILURE);
                }
                in_macro = 0;
                currentMacro = NULL; // End of macro, reset currentMacro
            }
// --------- Case 2: Inside Macro Declaration ---------- //
        } else if (in_macro) { // Case 2: Inside Macro Declaration
            if (currentMacro != NULL) {
                add_macro_content(currentMacro, line);
            }
        } else {
// --------- Case 3: Call for macro ------------------- //
            NodeOnList *calledMacro = find_macro(macroTable, identifier);
            if (calledMacro) {// Case 3: Call for macro
                NodeOfMacroContentList *content = calledMacro->Macro_content->head;
                while (content) {
                    fputs(content->line, am);
                    content = content->next;
                }
// --------- CASE 4: Start of macro decleration. ---------- //
            } else if (strstr(line, "macr") != NULL) { // CASE 4: Start of macro decleration.
                addNode(macroTable, line);
                in_macro = 1;
                currentMacro = macroTable->tail; // Set currentMacro to the newly added node
            }
// --------- Case 5: Noraml line. ----------------------- //
            else { // Case 5: Noraml line.
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
//       4) check that macro name isnt start..


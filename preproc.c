#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/preproc.h"
#include "headers/util.h"
#include "headers/globaldefine.h"
#include "headers/errors.h"


LinkedListOfMacro *macroTable = NULL;


int check_endmacr_format(const char *line)
{
    const char *ptr = strstr(line, "endmacr");
    if (!ptr)
    {
        return 0;
    }
    ptr += strlen("endmacr");
    while (*ptr)
    {
        if (!isspace((unsigned char)*ptr))
        {
            return 0;
        }
        ptr++;
    }
    return 1;
}

NodeOnList *find_macro(LinkedListOfMacro *macroTable, const char *name)
{
    NodeOnList *current;
    current = macroTable->head;
    if (!macroTable || !name)
        return NULL;
    while (current)
    {
        if (current->name && strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void process_file(char *as, char *am)
{
    int lineC;
    FILE *inputFile , *outputFile;
    char in_macro = 0;
    char line[MAX_LINE_LENGTH];
    location *amFile = NULL;
    NodeOnList *currentMacro = NULL;
    
    /* Initialize macroTable if not already done */
    if (macroTable == NULL) {
        macroTable = (LinkedListOfMacro *)handle_malloc(sizeof(LinkedListOfMacro));
        macroTable->head = NULL;
        macroTable->tail = NULL;
    }

    lineC = 0;
    amFile = (location *)handle_malloc(sizeof(location));
    if (amFile == NULL) {
        print_internal_error(ERROR_CODE_1);
        free(amFile);
        return;
    }

    inputFile = fopen(as,"r");
    outputFile = fopen(am,"wb");
    amFile->file_name = as;
    if (!as || !am)
    {
        print_internal_error(ERROR_CODE_2);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, MAX_LINE_LENGTH, inputFile) != NULL){
        char *identifier = Macro_name(line);
        lineC++;
        amFile->line_num = lineC;

        /*--------- CASE 1: End of macro decleration ----------*/
        if (strstr(line, "endmacr") != NULL)
        { /*CASE 1: End of macro decleration*/
            if (in_macro)
            { /*if inside of macro decleration and arrive "endmacr" line.*/
                if (!check_endmacr_format(line))
                {
                    print_internal_error(ERROR_CODE_10);
                    print_internal_error(ERROR_CODE_29);
                    return;
                }
                in_macro = 0;
                currentMacro = NULL; /*End of macro, reset currentMacro*/
            }
            /* --------- Case 2: Inside Macro Declaration ---------- */
        }
        else if (in_macro)
        { /*Case 2: Inside Macro Declaration*/
            if (currentMacro != NULL)
            {
                add_macro_content(currentMacro, line);
            }
        }
        else
        {
            /* --------- Case 3: Call for macro ------------------- */
            NodeOnList *calledMacro = find_macro(macroTable, identifier);
            if (calledMacro)
            { /*Case 3: Call for macro*/
                NodeOfMacroContentList *content = calledMacro->Macro_content->head;
                while (content)
                {
                    fputs(content->line, outputFile);
                    content = content->next;
                }
                /* --------- CASE 4: Start of macro decleration. ---------- */
            }
            else if (strstr(line, "macr") != NULL){ /*CASE 4: Start of macro decleration.*/
                if (!check_macro_declaration_format(line)) {
                    print_internal_error(ERROR_CODE_9);
                    print_internal_error(ERROR_CODE_29);
                    return;
                }
                addNode(macroTable, line);
                in_macro = 1;
                currentMacro = macroTable->tail; /*Set currentMacro to the newly added node*/
            }
            /* --------- Case 5: Noraml line. ----------------------- */
            else
            { /*Case 5: Noraml line.*/
                fputs(line, outputFile);
            }
        }
    }
    /*free_linked_list(macroTable);*/
    fclose(inputFile);
    fclose(outputFile);
}

/*TODO : 1) check if in the end of decleration "endmacr" there is not extra text ( Done).
       2) create method to replace the text when call for macro.
       3) check there is no calls for non existent macro.
       4) check that macro name isnt start..
       */

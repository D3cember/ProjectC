#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/preproc.h"
#include "../headers/util.h"
#include "../headers/globaldefine.h"
#include "../headers/errors.h"


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

void process_file(char *source_file, char *output_file_name)
{
    int line_count;
    FILE *input_file, *output_file;
    char is_in_macro = 0;
    char line_buffer[MAX_LINE_LENGTH];
    location *output_location = NULL;
    NodeOnList *current_macro_node = NULL;
    char *macro_name = NULL;
    char *newline_position = NULL;
    NodeOnList *found_macro = NULL;
    char *trimmed_line = NULL;
    NodeOfMacroContentList *macro_content_node = NULL;

    /* Initialize the macro table if it hasn't been done already */
    if (macroTable == NULL) {
        macroTable = (LinkedListOfMacro *)handle_malloc(sizeof(LinkedListOfMacro));
        macroTable->head = NULL;
        macroTable->tail = NULL;
    }

    line_count = 0;
    output_location = (location *)handle_malloc(sizeof(location));
    if (output_location == NULL) {
        print_internal_error(ERROR_CODE_1);
        free(output_location);
        return;
    }

    input_file = fopen(source_file, "r");
    output_file = fopen(output_file_name, "wb");
    output_location->file_name = source_file;
    if (input_file == NULL || output_file == NULL){
        print_internal_error(ERROR_CODE_2);

        if (input_file != NULL){
            fclose(input_file);
        } 
        if (output_file != NULL){
         fclose(output_file);
        }
        free(output_location);
        exit(EXIT_FAILURE);
    }


    while (fgets(line_buffer, MAX_LINE_LENGTH, input_file) != NULL)
    {
        macro_name = Macro_name(line_buffer);
        line_count++;
        output_location->line_num = line_count;

        /* Remove the newline character at the end of the line */
        newline_position = strchr(line_buffer, '\n');
        if (newline_position) {
            *newline_position = '\0';
        }

        /*--------- CASE 1: End of macro declaration ----------*/
        if (strstr(line_buffer, "endmacr") != NULL)
        {
            if (is_in_macro)
            {
                if (!check_endmacr_format(line_buffer))
                {
                    print_internal_error(ERROR_CODE_10);
                    print_internal_error(ERROR_CODE_29);
                    return;
                }
                is_in_macro = 0;
                current_macro_node = NULL;
            }
        }
        else if (is_in_macro)
        {
            if (current_macro_node != NULL)
            {
                add_macro_content(current_macro_node, line_buffer);
            }
        }
        else
        {
            found_macro = find_macro(macroTable, macro_name);
            if (found_macro)
            {
                /* Check if the line contains only the identifier */
                trimmed_line = line_buffer;
                while (*trimmed_line == ' ' || *trimmed_line == '\t')
                    trimmed_line++; 
                if (strcmp(trimmed_line, macro_name) == 0)
                {
                    /* Case 3: Call for macro */
                    macro_content_node = found_macro->Macro_content->head;
                    while (macro_content_node)
                    {
                        fputs(macro_content_node->line, output_file);
                        fputs("\n", output_file);  /* Ensure each macro line ends with a newline */
                        macro_content_node = macro_content_node->next;
                    }
                }
                else
                {
                    output_location->colo = macro_name;
                    print_external_error(9, *output_location);
                    print_internal_error(ERROR_CODE_39);
                    return;
                }
            }
            else if (strstr(line_buffer, "macr") != NULL)
            {
                if (!check_macro_declaration_format(line_buffer))
                {
                    print_internal_error(ERROR_CODE_9);
                    print_internal_error(ERROR_CODE_29);
                    return;
                }
                addNode(macroTable, line_buffer);
                is_in_macro = 1;
                current_macro_node = macroTable->tail;
            }
            else
            {
                fputs(line_buffer, output_file);
                fputs("\n", output_file); 
            }
        }
    }

    fclose(input_file);
    fclose(output_file);
    free(output_location);
}





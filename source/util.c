#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "../headers/util.h"
#include "../headers/errors.h"
#include "../headers/first_pass.h"
#include "../headers/second_pass.h"


const char *reserved_keywords[MAX_KEYWORDS] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop",".data",".string",".entry",".extern","r0","r1","r2","r3","r4","r5","r6","r7","macr","endmacr"};
char* Macro_name(const char *line) {
    const char *macro_declare = "macr";
    const char *temp_line = line;
    char *identifier = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
    char *temp_id = identifier;

    /*Skip leading whitespaces*/ 
    while (isspace(*temp_line)) {
        temp_line++;
    }

    /*Check if "macr" is present*/ 
    if (strncmp(temp_line, macro_declare, strlen(macro_declare)) == 0) {
        temp_line += strlen(macro_declare);
    }

    /*Skip whitespaces after "macr"*/ 
    while (isspace(*temp_line)) {
        temp_line++;
    }

    /*Copy the identifier*/ 
    while (*temp_line != '\n' && !isspace(*temp_line)) {
        *temp_id++ = *temp_line++;
    }
    *temp_id = '\0';
    

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
    const char *Macroformat = "macr";
    const char *temp_line = line;
    
    /*Skip leading whitespaces*/
    while (isspace(*temp_line)) {
        temp_line++;
    }

    /* Check if "macr" is present at the beginning */
    if (strncmp(temp_line, Macroformat, strlen(Macroformat)) != 0) {
        return 0;
    }
    temp_line += strlen(Macroformat);

    /* Skip whitespaces after "macr" */
    while (isspace(*temp_line)) {
        temp_line++;
    }

    /* Check if macro name is valid */
    if (!*temp_line || !isalpha((unsigned char)*temp_line)) {
        return 0;
    }

    /* Move ptr to the end of the macro name */
    while (*temp_line && (isalnum((unsigned char)*temp_line) || *temp_line == '_')) {
        temp_line++;
    }

    /* Ensure no extra non-whitespace characters after the macro name */
    while (*temp_line) {
        if (!isspace(*temp_line)) {
            return 0;
        }
        temp_line++;
    }

    return 1;
}

char *format_operands(char *input_operands) {
    static char formated_operands[256];
    char *input_ptr = input_operands;
    char *output_ptr = formated_operands;

    while (*input_ptr) {
        if (!isspace(*input_ptr) && *input_ptr != ',') {
            *output_ptr++ = *input_ptr;
        } else if (*input_ptr == ',') {
            /* Add comma and skip spaces after it */
            *output_ptr++ = ',';
            while (isspace(*(++input_ptr))) {}
            input_ptr--;  /* Adjust for the increment in the while loop */
        }
        input_ptr++;
    }
    *output_ptr = '\0';  /* Null-terminate the cleaned string */

    return formated_operands;
}


int get_operand_type(const char *operand) {
    /* Check for immediate addressing */
    if (operand[0] == '#') {
        if (isdigit(operand[1]) || (operand[1] == '-' && isdigit(operand[2]))) {
            return 0;  /* Immediate addressing */
        } else {
            return -1;  /* Invalid operand type if not followed by a number */
        }
    }
    /* Check for register direct addressing (r0-r7) */
    else if (operand[0] == 'r' && strlen(operand) == 2 && isdigit(operand[1]) && operand[1] >= '0' && operand[1] <= '7') {
        return 3;  /* Register direct addressing */
    }
    /* Check for indirect addressing (e.g., *r0) */
    else if (operand[0] == '*' && operand[1] == 'r' && strlen(operand) == 3 && isdigit(operand[2]) && operand[2] >= '0' && operand[2] <= '7') {
        return 2;  /* Indirect addressing through register */
    }
    /* Check for direct addressing (labels or variable names) */
    else if (isalpha(operand[0])) {
        return 1;  /* Direct addressing */
    }
    /* If none of the above, return invalid type */
    return -1;  /* Invalid operand type */
}



int analyze_line(char *line, char **label, char **instruction, char **operand, location *amFile, int *errC) {
    char *line_start = line;
    char *line_end;
    char *colon_position;
    char *comment_start;

    /* Skip leading spaces or tabs */
    while (*line_start == ' ' || *line_start == '\t') line_start++;

    /* Check if the first non-whitespace character is a semicolon */
    if (*line_start == ';') {
        return 1;  /* Indicate that this is a comment line */
    }

    /* Trim trailing whitespace */
    line_end = line_start + strlen(line_start) - 1;
    while (line_end > line_start && (*line_end == ' ' || *line_end == '\t' || *line_end == '\n')) {
        line_end--;
    }
    *(line_end + 1) = '\0';

    /* Handle inline comments (anything after a semicolon) */
    comment_start = strchr(line_start, ';');
    if (comment_start) {
        *comment_start = '\0';  /* Null-terminate the line at the semicolon */
        
        /* After removing the comment, check if the line is empty */
        if (*line_start == '\0') {
            return 1;  /* Indicate that this is a comment line */
        }
    }

    /* Initialize pointers for label, instruction, and operand */
    *label = NULL;
    *instruction = NULL;
    *operand = NULL;

    /* Check for label by looking for a colon */
    colon_position = strchr(line_start, ':');
    if (colon_position) {
        *colon_position = '\0';  /* Split the string at the colon */
        *label = line_start;  /* The part before the colon is the label */

        /* Validate the label */
        if (!is_valid_label(*label)) {
            amFile->colo = *label;
            print_external_error(19, *amFile);  /* Invalid label */
            (*errC)++;
            return -1;
        }

        /* Move to the part after the colon and remove leading whitespace */
        line_start = colon_position + 1;
        while (*line_start == ' ' || *line_start == '\t') {
            line_start++;
        }
    }

    if (*line_start != '\0') {
        /* Parse the instruction */
        *instruction = line_start;
        while (*line_start != ' ' && *line_start != '\t' && *line_start != '\0') {
            line_start++;
        }

        if (*line_start != '\0') {
            *line_start = '\0';  /* Null-terminate the instruction */
            line_start++;
            /* Skip leading spaces or tabs in the operand */
            while (*line_start == ' ' || *line_start == '\t') {
                line_start++;
            }
            *operand = line_start;  /* Set operand to the rest of the line */
            
            /* Format the operands */
            *operand = format_operands(*operand);  /* Format the operand to remove extra spaces */
        }

        return 0;  /* Indicate that this is not a comment line */
    }

    return -1;  /* Indicate some kind of error or empty line */
}

InstructionMap instruction_map[] = {
    {"mov", MOV, 0},
    {"cmp", CMP, 1},
    {"add", ADD, 2},
    {"sub", SUB, 3},
    {"lea", LEA, 4},
    {"clr", CLR, 5},
    {"not", NOT, 6},
    {"inc", INC, 7},
    {"dec", DEC, 8},
    {"jmp", JMP, 9},
    {"bne", BNE, 10},
    {"red", RED, 11},
    {"prn", PRN, 12},
    {"jsr", JSR, 13},
    {"rts", RTS, 14},
    {"stop", STOP, 15},
};

InstructionType get_instruction_type(const char *instruction) {
    int i;
    for (i = 0; i <= INSTRUCTION_COUNT; i++) {
        if (strcmp(instruction, instruction_map[i].name) == 0) {
            return instruction_map[i].type;
        }
    }
    return UNKNOWN;
}

int get_opcode(const char *instruction) {
    int i;
    for (i = 0; i <= INSTRUCTION_COUNT; i++) {
        if (strcmp(instruction, instruction_map[i].name) == 0) {
            return instruction_map[i].opcode;
        }
    }
    return -1;  /* Invalid instruction */
}
int set_operand_bits(int operandType, int isSource) {
    int value = 0;
    switch (operandType) {
        case 0:  /* Immediate */
            value = 1;  /* ביט 0 מייצג Immediate */
            break;
        case 1:  /* Direct */
            value = 2;  /* ביט 1 מייצג Direct */
            break;
        case 2:  /* Register Indirect */
            value = 4;  /* ביט 2 מייצג Register Indirect */
            break;
        case 3:  /* Register Direct */
            value = 8;  /* ביט 3 מייצג Register Direct */
            break;
        default:
            return 0;  /* במקרה של שגיאה */
    }
    if (isSource) {
        return value << 7;  /* קידוד בביטים 10-7 */
    } else {
        return value << 3;  /* קידוד בביטים 6-3 */
    }
}

Symbol* find_symbol(const char *label) {
    Symbol *current_label_name = symbol_table;
    while (current_label_name != NULL) {
        if (strcmp(label, current_label_name->label) == 0) {
            return current_label_name;  /* מחזיר את המצביע לסמל שנמצא */
        }
        current_label_name = current_label_name->next;  /* מעבר לסמל הבא */
    }
    return NULL;  /* הסמל לא נמצא */
}

int is_register(int operand_type, const char *operand) {
    if (operand_type == 2 || operand_type == 3) {
        /* בדיקת רגיסטר ישיר (r0 עד r7) */
        if (operand_type == 3 && operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0') {
            return 1;
        }
        /* בדיקת רגיסטר עקיף (*r0 עד *r7) */
        if (operand_type == 2 && operand[0] == '*' && operand[1] == 'r' && operand[2] >= '0' && operand[2] <= '7' && operand[3] == '\0') {
            return 1;
        }
    }
    return 0;  /* לא רגיסטר חוקי */
}


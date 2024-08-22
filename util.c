#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "headers/util.h"
#include "headers/errors.h"
#include "headers/preproc.h"


const char *reserved_keywords[MAX_KEYWORDS] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop",".data",".string",".entry",".extern","r0","r1","r2","r3","r4","r5","r6","r7","macr","endmacr"};
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
int check_macro_declaration_format(const char *line) {
    const char *macr = "macr";
    const char *ptr = line;
    
    /*Skip leading whitespaces*/
    while (isspace(*ptr)) {
        ptr++;
    }

    /* Check if "macr" is present at the beginning */
    if (strncmp(ptr, macr, strlen(macr)) != 0) {
        return 0;
    }
    ptr += strlen(macr);

    /* Skip whitespaces after "macr" */
    while (isspace(*ptr)) {
        ptr++;
    }

    /* Check if macro name is valid */
    if (!*ptr || !isalpha((unsigned char)*ptr)) {
        return 0;
    }

    /* Move ptr to the end of the macro name */
    while (*ptr && (isalnum((unsigned char)*ptr) || *ptr == '_')) {
        ptr++;
    }

    /* Ensure no extra non-whitespace characters after the macro name */
    while (*ptr) {
        if (!isspace(*ptr)) {
            return 0;
        }
        ptr++;
    }

    return 1;
}

char *format_operands(char *operand_str) {
    static char formatted_operands[256];
    char *src = operand_str;
    char *dst = formatted_operands;

    while (*src) {
        if (!isspace(*src) && *src != ',') {
            *dst++ = *src;
        } else if (*src == ',') {
            /* Add comma and skip spaces after it */
            *dst++ = ',';
            while (isspace(*(++src))) {}
            src--;  /* Adjust for the increment in the while loop */
        }
        src++;
    }
    *dst = '\0';  /* Null-terminate the formatted string */

    return formatted_operands;
}

int get_operand_type(const char *operand) {
    /* Check for immediate addressing */
    if (operand[0] == '#') {
        return 0;  /* Immediate addressing */
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

void analyze_line(char *line, char **label, char **instruction, char **operand, location *amFile, int *errC) {
    char *line_start;
    char *line_end;
    char *colon_position;

    /* Trim leading and trailing whitespace */
    line_start = line;  /* line_start points to the beginning of the line */
    line_end = line_start + strlen(line_start) - 1;  /* line_end points to the last character */

    while (*line_start == ' ' || *line_start == '\t') line_start++;  /* Skip leading spaces or tabs */
    while (line_end > line_start && (*line_end == ' ' || *line_end == '\t' || *line_end == '\n')) line_end--;  /* Skip trailing spaces, tabs, or newlines */
    *(line_end + 1) = '\0';  /* Null-terminate the trimmed string */

    if (*line_start == '\0' || *line_start == ';') {
        return;  /* Skip empty lines or comments */
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
            amFile->colo = *label;  /* Set the label as the column in the location structure */
            print_external_error(19, *amFile);  /* Invalid label */
            (*errC)++;  /* Increment error count */
            return;  /* Skip further processing if label is invalid */
        }

        /* Move to the part after the colon and remove leading whitespace */
        line_start = colon_position + 1;
        while (*line_start == ' ' || *line_start == '\t') {
            line_start++;
        }
    } else if (line[0] != ' ') {
        /* If no colon is found and the line doesn't start with a space, treat the first word as a potential label */
        *label = strtok(line_start, " \t");
        amFile->colo = *label;  /* Set the label as the column in the location structure */
        print_external_error(19, *amFile);  /* Error: Missing colon in label */
        (*errC)++;  /* Increment error count */
        return;  /* Skip this line and move to the next one */
    }

    if (*line_start != '\0') {
        /* Manually parse the instruction */
        *instruction = line_start;
        while (*line_start != ' ' && *line_start != '\t' && *line_start != '\0') {
            line_start++;  /* Move to the end of the instruction */
        }

        if (*line_start != '\0') {
            *line_start = '\0';  /* Null-terminate the instruction */
            line_start++;  /* Move to the operand part */
            while (*line_start == ' ' || *line_start == '\t') {
                line_start++;  /* Skip leading spaces or tabs in the operand */
            }
            *operand = format_operands(line_start);  /* Set operand to the rest of the line */
        }
    }
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
    {"jsr", JSR, 13},
    {"red", RED, 11},
    {"prn", PRN, 12},
    {"rts", RTS, 14},
    {"stop", STOP, 15}
};

InstructionType get_instruction_type(const char *instruction) {
    int i;
    for (i = 0; i < INSTRUCTION_COUNT; i++) {
        if (strcmp(instruction, instruction_map[i].name) == 0) {
            return instruction_map[i].type;
        }
    }
    return UNKNOWN;
}

int get_opcode(const char *instruction) {
    int i;
    for (i = 0; i < INSTRUCTION_COUNT; i++) {
        if (strcmp(instruction, instruction_map[i].name) == 0) {
            return instruction_map[i].opcode;
        }
    }
    return -1;  /* Invalid instruction */
}














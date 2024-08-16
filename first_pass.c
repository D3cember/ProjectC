#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "headers/data_struct.h"
#include "headers/preproc.h"
#include "headers/errors.h"
#include "headers/globaldefine.h"
#include "headers/util.h"

int error_detect = 0;

int count_data_items(const char *data) {
    int count = 1;
    const char *p = data;
    while ((p = strchr(p, ',')) != NULL) {
        count++;
        p++;
    }
    return count;
}

const char *instr[OPERATION_AMMOUNT] = {"add", "sub", "mov", "cmp", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts","stop"};
int instructionCheck(const char *word) {
    int i;
    for (i = 0; i < OPERATION_AMMOUNT; i++) { 
        if (strcmp(word, instr[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


void first_pass(char *filename) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int IC = IC_START, DC = DC_START;
    char *label = NULL;
    char *instruction = NULL;
    char *operand = NULL;
    int lineC = 0;
    int errC = 0;
    location *amFile = NULL;

    /* Open the file for reading */
    file = fopen(filename, "r");
    if (!file) {
        /* Handle error if the file cannot be opened */
        print_internal_error(ERROR_CODE_4);
        return;
    }

    /* Allocate memory for the location structure */
    amFile = (location *)handle_malloc(sizeof(location));
    if (amFile == NULL) {
        /* Handle memory allocation failure */
        print_internal_error(ERROR_CODE_1);
        free(amFile);
        return;
    }

    /* Loop through each line in the file */
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        char *line_start;  /* Declare line_start at the beginning of the block */
        char *line_end;    /* Declare line_end at the beginning of the block */
        char *colon_position;  /* Declare colon_position at the beginning of the block */

        lineC++;  /* Increment the line counter */
        amFile->file_name = filename;  /* Set the filename in the location structure */
        amFile->line_num = lineC;  /* Set the line number in the location structure */

        /* Trim leading and trailing whitespace */
        line_start = line;  /* line_start points to the beginning of the line */
        line_end = line_start + strlen(line_start) - 1;  /* line_end points to the last character */

        while (*line_start == ' ' || *line_start == '\t') line_start++;  /* Skip leading spaces or tabs */
        while (line_end > line_start && (*line_end == ' ' || *line_end == '\t' || *line_end == '\n')) line_end--;  /* Skip trailing spaces, tabs, or newlines */
        *(line_end + 1) = '\0';  /* Null-terminate the trimmed string */

        if (*line_start == '\0' || *line_start == ';') {
            continue;  /* Skip empty lines or comments */
        }

        /* Initialize pointers for label, instruction, and operand */
        label = NULL;
        instruction = NULL;
        operand = NULL;

        /* Check for label by looking for a colon */
        colon_position = strchr(line_start, ':');
        if (colon_position) {
            *colon_position = '\0';  /* Split the string at the colon */
            label = line_start;  /* The part before the colon is the label */

            /* Validate the label */
            if (!is_valid_label(label)) {
                amFile->col = label;  /* Set the label as the column in the location structure */
                print_external_error(19, *amFile);  /* Invalid label */
                errC++;  /* Increment error count */
                continue;  /* Skip further processing if label is invalid */
            }

            /* Move to the part after the colon and remove leading whitespace */
            line_start = colon_position + 1;
            while (*line_start == ' ' || *line_start == '\t') {
                line_start++;
            }
        } else if (line[0] != ' ') {
            /* If no colon is found and the line doesn't start with a space, treat the first word as a potential label */
            label = strtok(line_start, " \t");
            amFile->col = label;  /* Set the label as the column in the location structure */
            print_external_error(19, *amFile);  /* Error: Missing colon in label */
            errC++;  /* Increment error count */
            continue;  /* Skip this line and move to the next one */
        }

        if (*line_start != '\0') {
            /* Manually parse the instruction */
            instruction = line_start;
            while (*line_start != ' ' && *line_start != '\t' && *line_start != '\0') {
                line_start++;  /* Move to the end of the instruction */
            }

            if (*line_start != '\0') {
                *line_start = '\0';  /* Null-terminate the instruction */
                line_start++;  /* Move to the operand part */
                while (*line_start == ' ' || *line_start == '\t') {
                    line_start++;  /* Skip leading spaces or tabs in the operand */
                }
                operand = format_operands(line_start);  /* Set operand to the rest of the line */
            }
        }

        /* Debugging output */
        printf("Label: %s, Instruction: %s, Operand: %s\n", label ? label : "NULL", instruction ? instruction : "NULL", operand ? operand : "NULL");

        /* Process the instruction and operands */
        if (instruction) {
            /* Handle various types of directives */
            if (strcmp(instruction, ".data") == 0) {
                if (label) {
                    add_symbol(label, DC + IC, 0);  /* Add the symbol for data directive */
                }
                encode_data(operand, &DC);  /* Encode the data */
            } else if (strcmp(instruction, ".string") == 0) {
                if (label) {
                    add_symbol(label, DC + IC, 0);  /* Add the symbol for string directive */
                }
                encode_string(operand, &DC);  /* Encode the string */
            } else if (strcmp(instruction, ".entry") == 0) {
                while ((operand = strtok(operand, ", ")) != NULL) {
                    handle_entry(operand);  /* Handle entry directive */
                }
            } else if (strcmp(instruction, ".extern") == 0) {
                while ((operand = strtok(operand, ", ")) != NULL) {
                    handle_extern(operand);  /* Handle extern directive */
                }
            } else {
                /* It's an actual instruction, not a directive */
                if (instruction && instructionCheck(instruction)) {
                    int instr_len = instructionLength(instruction, operand);
                    if (instr_len == -1) {
                        amFile->col = instruction;
                        print_external_error(18, *amFile);  /* Error: Invalid instruction length */
                        errC++;  /* Increment error count */
                        continue;  /* Skip further processing */
                    }
                    /* Only add the symbol if the instruction length is valid */
                    if (label) {
                        add_symbol(label, IC, 0);  /* Add the symbol for the instruction */
                    }
                    IC += instr_len;  /* Increment the instruction counter */
                    printf("Instruction: %s, new IC: %d\n", instruction, IC);  /* Debugging output */
                } else {
                    if (instruction) {
                        amFile->col = instruction;  /* Set the instruction as the column in the location structure */
                        print_external_error(13, *amFile);  /* Unknown instruction error */
                        errC++;  /* Increment error count */
                    }
                }
            }
        }
    }
    if (errC == 0) {
        adjust_data_symbols(IC);
    }
    else if (errC > 0){
        error_detect = errC;
        print_external_error(20,*amFile);
        return;
    }
        free(macroTable);

    fclose(file);
}






/* פונקציה להוספת 100 לערכים בטבלת הסמלים שאופיינו כנתונים */
void adjust_data_symbols(int IC) {
    Symbol *current = symbol_table;
    while (current) {
        if (current->address >= IC_START) {
            current->address += IC_BASE;
        }
        current = current->next;
    }
}



void encode_data(const char *operand, int *DC) {
    int value;
    const char *p;
    char *endptr;

    p = operand;
    while (p != NULL) {
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '+' || *p == '-' || (*p >= '0' && *p <= '9')) {
            value = strtol(p, &endptr, 10);
            if (p == endptr) {
                print_internal_error(ERROR_CODE_9);
                return;
            }
            printf("Encoded data at %d: %d\n", *DC + IC_START, value);
            (*DC)++;
            p = endptr;
        } else {
            print_internal_error(ERROR_CODE_9);
            return;
        }

        p = strchr(p, ',');
        if (p != NULL) p++;
    }
}

/* פונקציה לקידוד מחרוזות בזיכרון */
void encode_string(const char *operand, int *DC) {
    int length, i;
    const char *p;

    p = operand;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '\"') {
        p++;
    }

    length = strlen(p);
    for (i = 0; i < length && p[i] != '\"'; i++) {
        if ((p[i] < 32 || p[i] > 126) && p[i] != '\t') {
            print_internal_error(ERROR_CODE_14);
            return;
        }
        printf("Encoded string at %d: %c\n", *DC + IC_START, p[i]);
        (*DC)++;
    }

    printf("Encoded string at %d: \\0\n", *DC + IC_START);
    (*DC)++;
}


int instructionLength(const char *instruction, const char *operand) {
    int length = 1;  
    char *op;
    int operand_count = 0;
    int operand_types[2];  /* Supporting up to 2 operands */

    /* Tokenize and count operands, and get their types */
    op = strtok((char *)operand, ", ");
    while (op != NULL && operand_count < 2) {
        operand_types[operand_count] = get_operand_type(op);
        if (operand_types[operand_count] == -1) {
            return -1;  /* Invalid operand type */
        }
        operand_count++;
        op = strtok(NULL, ", ");
    }

    /* Validate the number of operands and their types for each instruction */
    if (strcmp(instruction, "mov") == 0 || strcmp(instruction, "cmp") == 0 || 
        strcmp(instruction, "add") == 0 || strcmp(instruction, "sub") == 0) {

        if (operand_count != 2) {
            return -1;  /* Must have exactly 2 operands */
        }
        /* In `mov`, `add`, and `sub`, destination can't be immediate (type 0), 
           but `cmp` allows immediate values for both source and destination. */
        if ((strcmp(instruction, "mov") == 0 || strcmp(instruction, "add") == 0 || strcmp(instruction, "sub") == 0) && operand_types[1] == 0) {
            return -1;  /* Destination can't be immediate for these instructions */
        }
        length += 2;

    } else if (strcmp(instruction, "lea") == 0) {
        if (operand_count != 2 || operand_types[0] != 1 || operand_types[1] == 0) {
            return -1;  /* LEA requires direct for source and not immediate for destination */
        }
        length += 2;

    } else if (strcmp(instruction, "clr") == 0 || strcmp(instruction, "not") == 0 || 
               strcmp(instruction, "inc") == 0 || strcmp(instruction, "dec") == 0 ||
               strcmp(instruction, "jmp") == 0 || strcmp(instruction, "bne") == 0 || 
               strcmp(instruction, "jsr") == 0 || strcmp(instruction, "red") == 0 || 
               strcmp(instruction, "prn") == 0) {

        if (operand_count != 1) {
            return -1;
        }
        /* Validate operand type here according to specific rules for each instruction */
        length += 1;

    } else if (strcmp(instruction, "rts") == 0 || strcmp(instruction, "stop") == 0) {
        if (operand_count != 0) {
            return -1;
        }
    }

    return length;
}




int is_reserved_keyword(const char *label) {
    int i;
    for (i = 0; i < MAX_KEYWORDS; i++) {
        if (strcmp(label, reserved_keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* פונקציה לטיפול בתוויות .entry */
void handle_entry(const char *label) {
    Symbol *sym = symbol_table;
    while (sym != NULL) {
        if (strcmp(sym->name, label) == 0) {
            /* ניתן להוסיף טיפול ספציפי לתוויות entry אם נדרש */
            return;
        }
        sym = sym->next;
    }
    print_internal_error(ERROR_CODE_16); /* תווית לא מוגדרת */
}

void handle_extern(const char *label) {
    if (!add_symbol(label, 0, 1)) {
        print_internal_error(ERROR_CODE_17); /* תווית קיימת כבר עם שם אחר */
    }
}

int is_valid_label(const char *label) {
    int i;
    int len;
    char temp_label[MAX_LABEL_LENGTH];
    NodeOnList *current;

    len = strlen(label);

    /* Check if the first character is a letter */
    if (!isalpha(label[0])) {
        return 0;
    }

    /* Check each character in the label (except the last one, which is the colon) */
    for (i = 1; i < len; i++) {
        if (!isalnum(label[i])) {
            return 0;
        }
    }

    /* Remove the colon for reserved keyword check */
    strncpy(temp_label, label, len - 1);
    temp_label[len - 1] = '\0'; /* Null-terminate the label without the colon */

    /* Check if it's a reserved keyword */
    if (is_reserved_keyword(temp_label)) {
        return 0;
    }

    /* Check if it's a macro name */
    current = macroTable->head;
    while (current != NULL) {
        if (strcmp(current->name, temp_label) == 0) {
            return 0;
        }
        current = current->next;
    }

    return 1; /* The label is valid */
}










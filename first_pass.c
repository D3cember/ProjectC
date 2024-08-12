#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "headers/data_struct.h"
#include "headers/errors.h"
#include "headers/globaldefine.h"
#include "headers/util.h"


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
    char line[256];
    int IC, DC;
    char *label = NULL;
    char *instruction = NULL;
    char *operand = NULL;
    char *colon_pos;
    int lineC = 0;
    int errC = 0;
    char *p;
    location *amFile = NULL;

    file = fopen(filename, "r");
    if (!file) {
        print_internal_error(ERROR_CODE_4);
        return;
    }

    amFile = (location *)handle_malloc(sizeof(struct location));
    if (amFile == NULL) {
        print_internal_error(ERROR_CODE_1);
        free(amFile);
        return;
    }

    IC = IC_START;
    DC = DC_START;

    while (fgets(line, sizeof(line), file)) {
        lineC++;
        p = strtok(line, "\n");
        if (!p || line[0] == ';' || line[0] == '\0') {
            continue;
        }
        
        amFile->file_name = filename;
        amFile->line_num = lineC;
        
        label = NULL;
        instruction = NULL;
        operand = NULL;
        colon_pos = strchr(line, ':');
        if (colon_pos) {
            *colon_pos = '\0';
            label = line;
            instruction = colon_pos + 1;
            while (*instruction == ' ') instruction++;
        } else {
            // If a label exists but does not end with a colon, trigger an error
            operand = strtok(line, " ");
            if (operand && isalpha(operand[0]) && is_valid_label(operand)) {
                // If the operand looks like a label but does not have a colon
                amFile->col = operand;
                print_external_error(8, *amFile);
                errC++;
                continue;
            }
            instruction = line;
        }

        operand = strtok(instruction, " ");
        if (operand) {
            instruction = operand;
            operand = strtok(NULL, "");
        }
        amFile->col = label;
        if (label && is_reserved_keyword(label)) {
            print_external_error(8, *amFile);
        }
        if (strcmp(instruction, ".data") == 0 || strcmp(instruction, ".string") == 0) {
            if (label) {
                add_symbol(label, DC + IC, 0);
            }
            if (strcmp(instruction, ".data") == 0) {
                encode_data(operand, &DC);
            } else if (strcmp(instruction, ".string") == 0) {
                encode_string(operand, &DC);
            }
            continue;
        }

        if (strcmp(instruction, ".entry") == 0) {
            while ((operand = strtok(NULL, ", ")) != NULL) {
                handle_entry(operand);
            }
            continue;
        }

        if (strcmp(instruction, ".extern") == 0) {
            while ((operand = strtok(NULL, ", ")) != NULL) {
                handle_extern(operand);
            }
            continue;
        }

        if (label && instructionCheck(instruction)) {
            add_symbol(label, IC, 0);
        }

        amFile->col = instruction;

        if (instructionLength(instruction,operand) == -1){
            print_external_error(18,*amFile);
            errC++;
        }
        
        if (instructionCheck(instruction)) {
            IC += instructionLength(instruction, operand);
            printf("Instruction: %s, new IC: %d\n", instruction, IC);
        } else {
            print_external_error(13,*amFile);
            errC++;
        }
    }

    fclose(file);

    adjust_data_symbols(IC);
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

    op = strtok((char *)operand, ", ");
    while (op != NULL) {
        operand_count++;
        op = strtok(NULL, ", ");
    }

    if (strcmp(instruction, "mov") == 0 || strcmp(instruction, "cmp") == 0 || strcmp(instruction, "add") == 0 ||
        strcmp(instruction, "sub") == 0 || strcmp(instruction, "lea") == 0) {
        if (operand_count > 2) {
            return -1;  
        }
        length += 2;

    } else if (strcmp(instruction, "clr") == 0 || strcmp(instruction, "not") == 0 || strcmp(instruction, "inc") == 0 ||
               strcmp(instruction, "dec") == 0 || strcmp(instruction, "jmp") == 0 || strcmp(instruction, "bne") == 0 ||
               strcmp(instruction, "jsr") == 0 || strcmp(instruction, "red") == 0 || strcmp(instruction, "prn") == 0) {
        if (operand_count > 1) {
            return -1;  
        }
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
    char temp_label[MAX_LABEL_LENGTH];

    // בדיקה אם התו הראשון הוא אות
    if (!isalpha(label[0])) {
        return 0;
    }

    // בדיקה שכל התווים הם אותיות או ספרות, חוץ מהתו האחרון שיכול להיות ':'
    for (i = 1; label[i] != '\0'; i++) {
        if (!isalnum(label[i])) {
            if (label[i] == ':' && label[i + 1] == '\0') {
                break; // אם התו האחרון הוא ':' זה חוקי
            } else {
                return 0;
            }
        }
    }

    // העתקה של התווית למשתנה זמני עד לנקודת ':' או לסופה
    for (i = 0; label[i] != '\0' && label[i] != ':'; i++) {
        temp_label[i] = label[i];
    }
    temp_label[i] = '\0'; // הוספת תו סיום מחרוזת

    // בדיקה אם זו מילה שמורה
    if (is_reserved_keyword(temp_label)) {
        return 0;
    }

    return 1;
}






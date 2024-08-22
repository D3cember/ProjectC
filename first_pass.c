#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "headers/preproc.h"
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
    int IC = IC_START;
    int DC = DC_START;
    int lineC = 0;
    int errC = 0;
    char *label = NULL;
    char *instruction = NULL;
    char *operand = NULL;
    char *operand1 = NULL;
    char *operand2 = NULL;
    char *trimmed_line = NULL;
    location amFile;

    /* פותחים את הקובץ לקריאה */
    if (!(file = fopen(filename, "r"))) {
        print_internal_error(ERROR_CODE_2);
        return;
    }

    /* מעבר על כל השורות בקובץ */
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        lineC++;
        amFile.file_name = filename;
        amFile.line_num = lineC;

        operand1 = operand2 = NULL;

        /* דלג על שורות ריקות או תגובות */
        trimmed_line = line;
        if (*trimmed_line == '\0' || *trimmed_line == ';') continue;

        /* ניתוח השורה */
        analyze_line(trimmed_line, &label, &instruction, &operand, &amFile, &errC);

        /* בדוק אם אין הוראה בשורה */
        if (instruction == NULL) {
            amFile.colo = label;
            print_external_error(18, amFile);  /* Error: Missing instruction */
            errC++;
            continue;  /* דלג על השורה הנוכחית */
        }

        /* פיצול האופרנדים אם זה אינו אחת ההנחיות המיוחדות */
        if (operand && strcmp(instruction, ".data") != 0 && strcmp(instruction, ".string") != 0 && strcmp(instruction, ".entry") != 0 && strcmp(instruction, ".extern") != 0) {
            operand1 = strtok(operand, ",");
            operand2 = strtok(NULL, ",");
        } else {
            operand1 = operand;
        }

        /* Debugging output */
        printf("Label: %s, Instruction: %s, Operand: %s\n", label ? label : "NULL", instruction ? instruction : "NULL", operand ? operand : "NULL");

        /* Process the instruction and operands */
        if (instruction) {
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
                if (instructionCheck(instruction)) {
                    int instr_len = instructionLength(instruction, operand1, operand2, amFile);
                    if (instr_len == -1) {
                        errC++;
                        continue;
                    }
                    if (label) {
                        add_symbol(label, IC, 0);  /* Add the symbol for the instruction */
                    }
                    IC += instr_len;  /* Increment the instruction counter */
                    printf("Instruction: %s, new IC: %d\n", instruction, IC);  /* Debugging output */
                } else {
                    amFile.colo = instruction;
                    print_external_error(20, amFile);  /* Unknown instruction error */
                    errC++;
                }
            }
        }
    }
    if (errC == 0) {
        adjust_data_symbols(IC);
    } else if (errC > 0) {
        error_detect = errC;
        print_external_error(21, amFile);
    }

    free_linked_list(macroTable);
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



/* פונקציה לקידוד נתונים בזיכרון */
void encode_data(const char *operands, int *DC) {
    int value;
    const char *p;
    char *endptr;

    p = operands;
    while (p != NULL) {
        /* דלג על רווחים או טאבים */
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        /* בדיקת מספרים */
        if (*p == '+' || *p == '-' || (*p >= '0' && *p <= '9')) {
            value = strtol(p, &endptr, 10);
            if (p == endptr) {
                /* שגיאה אם הערך אינו תקין */
                print_internal_error(ERROR_CODE_22);
                return;
            }
            /* הדפסת הכתובת והערך המקודד */
            printf("Encoded data at %d: %d\n", *DC + IC_START, value);
            (*DC)++;  /* עדכון מונה הנתונים */
            p = endptr;
        } else {
            /* שגיאה אם יש תו שאינו מספרי */
            print_internal_error(ERROR_CODE_22);
            return;
        }

        /* מצא את הפסיק הבא והתקדם */
        p = strchr(p, ',');
        if (p != NULL) p++;
        else break;  /* יציאה מהלולאה אם אין יותר פסיקים */
    }
}

/* פונקציה לקידוד מחרוזות בזיכרון */
void encode_string(const char *operand, int *DC) {
    int length, i;
    const char *p;

    p = operand;
    /* דלג על רווחים או טאבים */
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '\"') {
        p++;  /* דלג על מרכאות פתיחה */
    }

    length = strlen(p);  /* קבלת אורך המחרוזת */
    for (i = 0; i < length && p[i] != '\"'; i++) {
        /* בדוק אם התו נמצא בתחום התקין */
        if ((p[i] < 32 || p[i] > 126) && p[i] != '\t') {
            /* שגיאה אם יש תו לא תקין */
            print_internal_error(ERROR_CODE_23);
            return;
        }
        /* הדפסת הכתובת והתו המקודד */
        printf("Encoded string at %d: %c\n", *DC + IC_START, p[i]);
        (*DC)++;  /* עדכון מונה הנתונים */
    }

    /* קידוד תו סיום המחרוזת */
    printf("Encoded string at %d: \\0\n", *DC + IC_START);
    (*DC)++;
}

int instructionLength(const char *instruction, const char *operand1, const char *operand2, location amFile) {
    int length = 1;
    int operand_count = 0;
    int operand_types[2];  /* תמיכה בעד 2 אופרנדים */
    InstructionType instr_type = get_instruction_type(instruction);
    char combined_operands[MAX_LINE_LENGTH];

    /* ניתוח האופרנדים בנפרד */
    if (operand1 != NULL) {
        operand_types[operand_count] = get_operand_type(operand1);
        if (operand_types[operand_count] == -1) {
            amFile.colo = operand1;
            print_external_error(24, amFile);  /* Error: Invalid operand type for source */
            return -1;
        }
        operand_count++;
    }

    if (operand2 != NULL) {
        operand_types[operand_count] = get_operand_type(operand2);
        if (operand_types[operand_count] == -1) {
            amFile.colo = operand2;
            print_external_error(25, amFile);  /* Error: Invalid operand type for destination */
            return -1;
        }
        operand_count++;
    }

    /* ניהול ההוראות לפי סוג */
    switch (instr_type) {
        case MOV:
        case ADD:
        case SUB:
            if (operand_count != 2) {
                amFile.colo = instruction;
                print_external_error(operand_count < 2 ? 27 : 26, amFile);  /* Error: Operand count mismatch */
                return -1;
            }
            /* בדיקה עבור MOV, ADD, SUB - המקור יכול להיות בשיטות 0,1,2,3, היעד יכול להיות בשיטות 1,2,3 בלבד */
            if (operand_types[1] == 0) {
                amFile.colo = operand2;
                print_external_error(25, amFile);  /* Error: Invalid destination operand type */
                return -1;
            }
            length += (operand_types[0] == 3 && operand_types[1] == 3) ? 1 : 2;
            break;

        case CMP:
            if (operand_count != 2) {
                amFile.colo = instruction;
                print_external_error(operand_count < 2 ? 27 : 26, amFile);  /* Error: Operand count mismatch */
                return -1;
            }
            /* עבור CMP - שני האופרנדים יכולים להיות בשיטות 0,1,2,3 */
            length += (operand_types[0] == 3 && operand_types[1] == 3) ? 1 : 2;
            break;

        case LEA:
            if (operand_count != 2) {
                amFile.colo = instruction;
                print_external_error(operand_count < 2 ? 27 : 26, amFile);  /* Error: Operand count mismatch */
                return -1;
            }
            /* עבור LEA - המקור חייב להיות בשיטת מיעון 1 בלבד, היעד יכול להיות בשיטות 1,2,3 */
            if (operand_types[0] != 1 || operand_types[1] == 0) {
                snprintf(combined_operands, sizeof(combined_operands), "%s %s", operand1, operand2);
                amFile.colo = combined_operands;
                print_external_error(28, amFile);  /* Error: Invalid operand types for LEA */
                return -1;
            }
            length += 2;
            break;

        case CLR:
        case NOT:
        case INC:
        case DEC:
            if (operand_count != 1) {
                amFile.colo = instruction;
                print_external_error(operand_count < 1 ? 27 : 26, amFile);  /* Error: Operand count mismatch */
                return -1;
            }
            /* עבור CLR, NOT, INC, DEC - היעד יכול להיות בשיטות 1,2,3 בלבד */
            if (!(operand_types[0] == 1 || operand_types[0] == 2 || operand_types[0] == 3)) {
                amFile.colo = operand1;
                print_external_error(28, amFile);  /* Error: Invalid operand type for instruction */
                return -1;
            }
            length += 1;
            break;

        case JMP:
        case BNE:
        case JSR:
            if (operand_count != 1) {
                amFile.colo = instruction;
                print_external_error(operand_count < 1 ? 27 : 26, amFile);  /* Error: Operand count mismatch */
                return -1;
            }
            /* עבור JMP, BNE, JSR - היעד יכול להיות בשיטות 1,2 בלבד */
            if (!(operand_types[0] == 1 || operand_types[0] == 2)) {
                amFile.colo = operand1;
                print_external_error(28, amFile);  /* Error: Invalid operand type for instruction */
                return -1;
            }
            length += 1;
            break;

        case PRN:
            if (operand_count != 1) {
                amFile.colo = instruction;
                print_external_error(operand_count < 1 ? 27 : 26, amFile);  /* Error: Operand count mismatch */
                return -1;
            }
            /* עבור PRN - היעד יכול להיות בשיטות 0,1,2,3 */
            if (!(operand_types[0] == 0 || operand_types[0] == 1 || operand_types[0] == 2 || operand_types[0] == 3)) {
                amFile.colo = operand1;
                print_external_error(28, amFile);  /* Error: Invalid operand type for PRN */
                return -1;
            }
            length += 1;
            break;

        case RED:
            if (operand_count != 1) {
                amFile.colo = instruction;
                print_external_error(operand_count < 1 ? 27 : 26, amFile);  /* Error: Operand count mismatch */
                return -1;
            }
            /* עבור RED - היעד יכול להיות בשיטות 1,2,3 בלבד */
            if (!(operand_types[0] == 1 || operand_types[0] == 2 || operand_types[0] == 3)) {
                amFile.colo = operand1;
                print_external_error(28, amFile);  /* Error: Invalid operand type for RED */
                return -1;
            }
            length += 1;
            break;

        case RTS:
        case STOP:
            if (operand_count != 0) {
                amFile.colo = instruction;
                print_external_error(29, amFile);  /* Error: Instruction should not receive any operands */
                return -1;
            }
            break;

        default:
            amFile.colo = instruction;
            print_external_error(20, amFile);  /* Error: Unknown instruction */
            return -1;
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

void handle_entry(const char *label) {
    Symbol *sym = symbol_table;
    while (sym != NULL) {
        if (strcmp(sym->label, label) == 0) {
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

/*int encoded_instruction(const char *operand1 , const char *operand2,const char *instruction){
    int encoded_value , opcode , srcSort , destSort , are , line;
    are = 4;



    


    

}*/








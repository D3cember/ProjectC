#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../headers/preproc.h"
#include "../headers/globaldefine.h"
#include "../headers/first_pass.h"
#include "../headers/util.h"
#include "../headers/files_handler.h"
#include "../headers/data_struct.h"
int error_detect = 0;

int IC;
int DC;
int SIC;
int SDC;
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
    int instr_len;
    int analysis_result;
    char *label = NULL;
    char *instruction = NULL;
    char *operand = NULL;
    char *operand1 = NULL;
    char *operand2 = NULL;
    char *trimmed_line;
    location amFile;
    InstructionInfo info;  /* הכרזה על משתנה */
    CodeNode *code_list = NULL;

    FILE *outputFile = fopen("output_binary_codes.txt", "w");
    if (outputFile == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    file = fopen(filename, "r");
    if (!file) {
        print_internal_error(ERROR_CODE_2);
        return;
    }

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        lineC++;
        amFile.file_name = filename;
        amFile.line_num = lineC;

        operand1 = operand2 = NULL;
        trimmed_line = line;

        /* בדוק אם השורה ריקה או שהיא הערה */
        while (*trimmed_line == ' ' || *trimmed_line == '\t' || *trimmed_line == '\n') trimmed_line++;
        if (*trimmed_line == ';' || *trimmed_line == '\0') {
            continue;  /* המשך לשורה הבאה אם מדובר בשורת הערה או שורה ריקה */
        }
        /* ניתוח השורה */
        analysis_result = analyze_line(trimmed_line, &label, &instruction, &operand, &amFile, &errC);
        
        /* בדוק אם השורה הייתה שורת הערה */
        if (analysis_result == 1) {
            continue;  /* סרוק את השורה החדשה */
        }

        if (instruction == NULL) {
            amFile.colo = label;
            print_external_error(18, amFile);
            errC++;
            continue;
        }

        if (operand && strcmp(instruction, ".data") != 0 && strcmp(instruction, ".string") != 0 &&
            strcmp(instruction, ".entry") != 0 && strcmp(instruction, ".extern") != 0) {
            operand1 = strtok(operand, ",");
            operand2 = strtok(NULL, ",");
        } else {
            operand1 = operand;
        }

        printf("Label: %s, Instruction: %s, Operand1: %s, Operand2: %s\n",
               label ? label : "NULL",
               instruction ? instruction : "NULL",
               operand1 ? operand1 : "NULL",
               operand2 ? operand2 : "NULL");

        if (instruction) {
            if (strcmp(instruction, ".data") == 0) {
                if (label) {
                    add_symbol(label, DC + IC, 0, 0);
                }
                encode_data(operand, &DC, &IC, &code_list);  
            } else if (strcmp(instruction, ".string") == 0) {
                if (label) {
                    add_symbol(label, DC + IC, 0, 0);
                }
                encode_string(operand, &DC, &IC, &code_list); 
            } else if (strcmp(instruction, ".entry") == 0) {
                handle_entry(operand, IC);
            } else if (strcmp(instruction, ".extern") == 0) {
                handle_extern(operand, IC);
            } else {
                if (instructionCheck(instruction)) {
                    info = instructionLength(instruction, &operand1, &operand2, amFile);
                    if (info.length == -1) {
                        errC++;
                        continue;
                    }
                    instr_len = info.length;
                    if (label) {
                        add_symbol(label, IC, 0, 0);
                    }

                    if (encodeInstruction(filename, instruction, operand1, operand2, IC, &code_list, amFile, 0) >= 0) {
                        /* שמירת המחרוזת הבינארית ברשימה המקושרת */
                    } else {
                        print_external_error(22, amFile);
                        errC++;
                    }
                    IC += instr_len;
                    printf("Instruction: %s, new IC: %d\n", instruction, IC);
                } else {
                    amFile.colo = instruction;
                    print_external_error(20, amFile);
                    errC++;
                }
            }
        }
    }
    SIC = IC;
    SDC = DC;

    if (errC == 0) {
        adjust_data_symbols(IC);
    } else if (errC > 0) {
        error_detect = errC;
        print_external_error(21, amFile);
    }

    fclose(outputFile);
    fclose(file);
}



/* פונקציה להוספת 100 לערכים בטבלת הסמלים שאופיינו כנתונים */
void adjust_data_symbols(int IC) {
    Symbol *current = symbol_table;
    while (current) {
        /* בדיקה אם התווית אינה מוגדרת כ-external ורק אז עדכון הכתובת */ 
        if (!current->is_external && current->address >= IC_START) {
            current->address += IC_BASE;
        }
        current = current->next;
    }
}

/* פונקציה לקידוד נתונים בזיכרון */
void encode_data(const char *operands, int *DC, int *IC, CodeNode **code_list) {
    int value;
    const char *p;
    char *endptr;
    char binary[16];

    p = operands;
    while (p != NULL) {
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '+' || *p == '-' || (*p >= '0' && *p <= '9')) {
            value = strtol(p, &endptr, 10);
            if (p == endptr) {
                print_internal_error(ERROR_CODE_22);
                return;
            }
            
            if (value < 0) {
                value = (1 << 15) + value;
            }

            int_to_binary(value, 15, binary);
            add_code_node(*DC + *IC + IC_BASE, binary, code_list);
            (*DC)++;
            p = endptr;
        } else {
            print_internal_error(ERROR_CODE_22);
            return;
        }

        p = strchr(p, ',');
        if (p != NULL) p++;
        else break;
    }
}

void encode_string(const char *operand, int *DC, int *IC, CodeNode **code_list) {
    int length, i;
    const char *p;
    char binary[16];

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
            print_internal_error(ERROR_CODE_23);
            return;
        }
        int_to_binary((int)p[i], 15, binary);
        add_code_node(*IC + *DC + IC_BASE, binary, code_list);
        (*DC)++;
    }

    int_to_binary(0, 15, binary);
    add_code_node(*IC + *DC + IC_BASE, binary, code_list);
    (*DC)++;
}


InstructionInfo instructionLength(const char *instruction, char **operand1, char **operand2, location amFile) {
    InstructionInfo info = {1, {-1, -1}};  /* ערכים התחלתיים */
    int operand_count = 0;
    InstructionType instr_type = get_instruction_type(instruction);
    char *temp;

    /* ניתוח האופרנדים בנפרד */
    if (*operand1 != NULL) {
        info.operand_types[0] = get_operand_type(*operand1);  /* קביעת סוג המיעון של המקור */
        if (info.operand_types[0] == -1) {
            amFile.colo = *operand1;
            print_external_error(24, amFile);  /* שגיאה: סוג אופרנד לא תקין עבור מקור */
            info.length = -1;
            return info;
        }
        operand_count++;
    }

    if (*operand2 != NULL) {
        info.operand_types[1] = get_operand_type(*operand2);  /* קביעת סוג המיעון של היעד */
        if (info.operand_types[1] == -1) {
            amFile.colo = *operand2;
            print_external_error(25, amFile);  /* שגיאה: סוג אופרנד לא תקין עבור יעד */
            info.length = -1;
            return info;
        }
        operand_count++;
    }

    /* אם יש רק אופרנד אחד, הוא תמיד היעד */
    if (operand_count == 1 && *operand1 != NULL) {  /* בדיקה אם operand1 אינו NULL */
        temp = (char *)malloc(strlen(*operand1) + 1);  /* הקצאת זיכרון לאופרנד היעד */
        if (temp == NULL) {
            print_internal_error(ERROR_CODE_3);  /* טיפול בשגיאת זיכרון */
            info.length = -1;
            return info;
        }
        strcpy(temp, *operand1);  /* העתקת ערך האופרנד הראשון ליעד */
        *operand2 = temp;  /* הגדרת האופרנד השני */
        *operand1 = NULL;  /* איפוס האופרנד הראשון */
        info.operand_types[1] = info.operand_types[0];
        info.operand_types[0] = -1;
    }

    /* ניהול ההוראות לפי סוג */
    switch (instr_type) {
        case MOV:
        case ADD:
        case SUB:
            if (operand_count != 2) {
                amFile.colo = instruction;
                print_external_error(operand_count < 2 ? 27 : 26, amFile);  /* שגיאה: אי התאמה במספר האופרנדים */
                info.length = -1;
                return info;
            }
            if (info.operand_types[1] == 0) {
                amFile.colo = *operand2;
                print_external_error(25, amFile);  /* שגיאה: סוג יעד לא תקין */
                info.length = -1;
                return info;
            }
            /* עדכון אורך ההוראה */
            info.length += (is_register(info.operand_types[0], *operand1) && is_register(info.operand_types[1], *operand2)) ? 1 : 2;
            break;

        case CMP:
            if (operand_count != 2) {
                amFile.colo = instruction;
                print_external_error(operand_count < 2 ? 27 : 26, amFile);  /* שגיאה: אי התאמה במספר האופרנדים */
                info.length = -1;
                return info;
            }
            /* עדכון אורך ההוראה */
            info.length += (is_register(info.operand_types[0], *operand1) && is_register(info.operand_types[1], *operand2)) ? 1 : 2;
            break;

        case LEA:
            if (operand_count != 2) {
                amFile.colo = instruction;
                print_external_error(operand_count < 2 ? 27 : 26, amFile);  /* שגיאה: אי התאמה במספר האופרנדים */
                info.length = -1;
                return info;
            }
            if (info.operand_types[0] != 1 || info.operand_types[1] == 0) {
                amFile.colo = *operand1;
                print_external_error(28, amFile);  /* שגיאה: סוגי אופרנדים לא תקינים עבור LEA */
                info.length = -1;
                return info;
            }
            info.length += 2;
            break;

        case CLR:
        case NOT:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case JSR:
        case PRN:
        case RED:
            if (operand_count != 1) {
                amFile.colo = instruction;
                print_external_error(operand_count < 1 ? 27 : 26, amFile);  /* שגיאה: אי התאמה במספר האופרנדים */
                info.length = -1;
                return info;
            }
            if (info.operand_types[1] == 0 && instr_type != PRN) { /* PRN יכול לקבל Immediate */
                amFile.colo = *operand2;
                print_external_error(28, amFile);  /* שגיאה: סוג יעד לא תקין עבור הפקודה */
                info.length = -1;
                return info;
            }
            info.length += 1;
            break;

        case RTS:
        case STOP:
            if (operand_count != 0) {
                amFile.colo = instruction;
                print_external_error(29, amFile);  /* שגיאה: הפקודה לא צריכה לקבל אופרנדים */
                info.length = -1;
                return info;
            }
            break;

        default:
            amFile.colo = instruction;
            print_external_error(20, amFile);  /* שגיאה: פקודה לא ידועה */
            info.length = -1;
            return info;
    }

    return info;
}

int is_reserved_keyword(char *label) {
    int i;
    for (i = 0; i < MAX_KEYWORDS; i++) {
        if (strcmp(label, reserved_keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void handle_entry(const char *label, int address) {
    Symbol *sym = symbol_table;
    while (sym != NULL) {
        if (strcmp(sym->label, label) == 0) {
            if (sym->is_external) {
                print_internal_error(ERROR_CODE_18); /* Error: Label cannot be both extern and entry */
                return;
            }
            sym->is_entry = 1;  /* Update the symbol to be an entry */
            return;
        }
        sym = sym->next;
    }

    /* If the label is not found, add it as an entry without changing the address */
    if (!add_symbol(label, address, 0, 1)) {
        print_internal_error(ERROR_CODE_16); /* Error: Label already exists */
    }
}


void handle_extern(const char *label, int address) {
    Symbol *sym = symbol_table;
    while (sym != NULL) {
        if (strcmp(sym->label, label) == 0) {
            if (sym->is_entry) {
                print_internal_error(ERROR_CODE_18); /* Error: Label cannot be both extern and entry */
                return;
            }
            sym->is_external = 1;  /* Update the symbol to be external */
            sym->address = 0; /* External symbols should have address 0 */
            return;
        }
        sym = sym->next;
    }

    /* If the label is not found, add it as an external symbol with address 0 */
    if (!add_symbol(label, 0, 1, 0)) {
        print_internal_error(ERROR_CODE_17); /* Error: Label already exists */
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

int encodeInstruction(const char *filename, const char *instruction, char *operand1, char *operand2, int IC, CodeNode **code_list, location amFile, int is_second_pass) {
    int opcode, srcModeBits = 0, destModeBits = 0, are;
    int srcMode, destMode;
    int encodedValue;
    char instructionBinary[16];
    int symbolFlag = 0;

    are = 4;

    opcode = get_opcode(instruction);
    if (opcode == -1) return -1;

    if (strcmp(instruction, "stop") == 0 || strcmp(instruction, "rts") == 0) {
        encodedValue = (opcode << 11) | are;
        int_to_binary(encodedValue, 15, instructionBinary);
        add_code_node(IC + IC_BASE, instructionBinary, code_list);
        return IC + 1;
    }

    srcMode = operand1 ? get_operand_type(operand1) : -1;
    destMode = operand2 ? get_operand_type(operand2) : -1;

    if (srcMode != -1) {
        srcModeBits = set_operand_bits(srcMode, 1);
    }
    if (destMode != -1) {
        destModeBits = set_operand_bits(destMode, 0);
    }

    encodedValue = (opcode << 11) | srcModeBits | destModeBits | are;

    int_to_binary(encodedValue, 15, instructionBinary);
    add_code_node(IC + IC_BASE, instructionBinary, code_list);
    IC++;

    if ((srcMode == 2 || srcMode == 3) && (destMode == 2 || destMode == 3)) {
        int regNumSrc = (srcMode == 2) ? operand1[2] - '0' : operand1[1] - '0';
        int regNumDest = (destMode == 2) ? operand2[2] - '0' : operand2[1] - '0';
        encodedValue = (regNumSrc << 6) | (regNumDest << 3) | are;

        int_to_binary(encodedValue, 15, instructionBinary);
        add_code_node(IC + IC_BASE, instructionBinary, code_list);
        IC++;
    } else {
        if (srcMode != -1) {
            encode_operand_value(filename, operand1, srcMode, 1, IC + IC_BASE, &symbolFlag, *code_list, is_second_pass);
            IC++;
        }

        if (destMode != -1) {
            encode_operand_value(filename, operand2, destMode, 0, IC + IC_BASE, &symbolFlag, *code_list, is_second_pass);
            IC++;
        }
    }
    return IC;
}


void encode_operand_value(const char *filename, const char *operand, int operandType, int isSource, int IC, int *symbolFlag, CodeNode *code_list, int is_second_pass) {
    int value = 0;
    int regNum = 0;
    int are = 4;
    int shiftAmount = 0;
    int encodedValue = 0;
    char binaryOutput[16];
    int i;
    Symbol *sym = NULL;

    switch (operandType) {
        case 0:  /* מיעון מיידי */
            value = atoi(&operand[1]);
            are = 4;
            encodedValue = (value & 0xFFF) << 3;
            break;

        case 1:  /* מיעון ישיר */
            sym = find_symbol(operand);
            if (sym != NULL) {
                if (sym->is_external) {
                    are = 1;
                    encodedValue = are;
                    if (is_second_pass) {
                        update_externals_file(filename, sym->label, IC);
                    }
                } else {
                    value = sym->address;
                    are = 2;
                    encodedValue = (value & 0xFFF) << 3;
                    encodedValue |= are;
                }
            } else {
                memset(binaryOutput, '0', 15);
                binaryOutput[15] = '\0';

                if (symbolFlag) {
                    *symbolFlag = 1;
                }
                add_code_node(IC, binaryOutput, &code_list);
                return;
            }
            break;

        case 2:  /* מיעון אוגר עקיף */
            regNum = operand[2] - '0';
            are = 4;
            shiftAmount = isSource ? 6 : 3;
            encodedValue = regNum << shiftAmount;
            break;

        case 3:  /* מיעון אוגר ישיר */
            regNum = operand[1] - '0';
            are = 4;
            shiftAmount = isSource ? 6 : 3;
            encodedValue = regNum << shiftAmount;
            break;
    }

    encodedValue |= are;

    memset(binaryOutput, '0', 15);
    binaryOutput[15] = '\0';

    for (i = 0; i < 15; i++) {
        binaryOutput[14 - i] = (encodedValue & (1 << i)) ? '1' : '0';
    }

    add_code_node(IC, binaryOutput, &code_list);
}

void int_to_binary(int value, int num_bits, char *binaryOutput) {
    int i;
    for (i = 0; i < num_bits; i++) {
        binaryOutput[i] = (value & (1 << (num_bits - 1 - i))) ? '1' : '0';
    }
    binaryOutput[num_bits] = '\0';  /* סיום המחרוזת */
}

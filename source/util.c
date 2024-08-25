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
    line_start = line;
    line_end = line_start + strlen(line_start) - 1;

    /* Skip leading spaces or tabs */
    while (*line_start == ' ' || *line_start == '\t') line_start++;
    /* Skip trailing spaces, tabs, or newlines */
    while (line_end > line_start && (*line_end == ' ' || *line_end == '\t' || *line_end == '\n')) line_end--;
    *(line_end + 1) = '\0';

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
            amFile->colo = *label;
            print_external_error(19, *amFile);  /* Invalid label */
            (*errC)++;
            return;
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
        }

        /* Handle special instructions like .entry and .extern */
        if (strcmp(*instruction, ".entry") == 0 || strcmp(*instruction, ".extern") == 0) {
            if (*operand == NULL || !is_valid_label(*operand)) {
                print_external_error(strcmp(*instruction, ".entry") == 0 ? 20 : 21, *amFile);  /* Invalid or missing operand */
                (*errC)++;
            }
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

void create_ent_file(const char *base_filename, Symbol *symbol_table) {
    char *ent_filename;
    FILE *ent_file;
    Symbol *sym;
    int has_entries = 0;

    /*בדוק אם יש תוויות עם is_entry = 1*/
    sym = symbol_table;
    while (sym != NULL) {
        if (sym->is_entry) {
            has_entries = 1;
            break;
        }
        sym = sym->next;
    }

    if (!has_entries) return; /* אם אין תוויות, אין צורך ליצור את הקובץ */

    /* יצירת שם הקובץ עם סיומת .ent */
    ent_filename = (char *)malloc(strlen(base_filename) + 5); /* 4 תווים עבור ".ent" ועוד אחד ל-null terminator */
    if (!ent_filename) {
        print_internal_error(ERROR_CODE_1); /* Error allocating memory */
        return;
    }
    strcpy(ent_filename, base_filename);
    strcat(ent_filename, ".ent");

    ent_file = fopen(ent_filename, "w");
    if (!ent_file) {
        print_internal_error(ERROR_CODE_2); /* Error opening file */
        free(ent_filename);
        return;
    }

    sym = symbol_table;
    while (sym != NULL) {
        if (sym->is_entry) {
            fprintf(ent_file, "%s %04d\n", sym->label, sym->address);
        }
        sym = sym->next;
    }

    fclose(ent_file);
    free(ent_filename);
}


void create_ext_file(const char *base_filename, Symbol *symbol_table) {
    char *ext_filename;
    FILE *ext_file;
    Symbol *sym;

    /* יצירת שם הקובץ עם סיומת .ext */
    ext_filename = (char *)malloc(strlen(base_filename) + 5);  /* 4 תווים עבור ".ext" ועוד אחד ל-null terminator */
    if (!ext_filename) {
        print_internal_error(ERROR_CODE_1);  /* Error allocating memory */
        return;
    }
    strcpy(ext_filename, base_filename);
    strcat(ext_filename, ".ext");

    ext_file = fopen(ext_filename, "w");
    if (!ext_file) {
        print_internal_error(ERROR_CODE_2);  /* Error opening file */
        free(ext_filename);
        return;
    }

    sym = symbol_table;
    while (sym != NULL) {
        if (sym->is_external) {
            fprintf(ext_file, "%s %04d\n", sym->label, sym->address);
        }
        sym = sym->next;
    }

    fclose(ext_file);
    free(ext_filename);
}

void create_ob_file(const char *filename, char binaryOutput[][MAX_BINARY_LENGTH], int IC, int DC) {
    FILE *ob_file;
    char *ob_filename;
    int i;
    ob_filename = NULL;
    snprintf(ob_filename, sizeof(ob_filename), "%s.ob", filename);
    ob_file = fopen(ob_filename, "w");
    if (ob_file) {
        fprintf(ob_file, "%04d %04d\n", IC, DC);  /* כתיבת כותרת עם ערכי IC ו-DC */
        for (i = 0; i < IC; i++) {
            fprintf(ob_file, "%04d %s\n", i + IC_START, binaryOutput[i]);  /* כתיבת קוד בינארי */
        }
        fclose(ob_file);
    }
}



/* פונקציה להוספת ערכים לקובץ .ext */
void add_to_ext_file(const char *filename, const char *label, int address) {
    char *ext_filename;
    FILE *ext_file;

    /* יצירת שם הקובץ עם סיומת .ext */
    ext_filename = (char *)malloc(strlen(filename) + 5); /* 4 תווים עבור ".ext" ועוד אחד ל-null terminator */
    if (!ext_filename) {
        print_internal_error(ERROR_CODE_1); /* שגיאה בהקצאת זיכרון */
        return;
    }
    strcpy(ext_filename, filename);
    strcat(ext_filename, ".ext");

    ext_file = fopen(ext_filename, "a");  /* פתיחה במצב append, כך שניתן להוסיף ערכים נוספים */
    if (!ext_file) {
        print_internal_error(ERROR_CODE_2); /* שגיאה בפתיחת קובץ */
        free(ext_filename);
        return;
    }

    fprintf(ext_file, "%s %04d\n", label, address);

    fclose(ext_file);
    free(ext_filename);
}







 


























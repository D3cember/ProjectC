#include "../headers/globaldefine.h"
#include "../headers/data_struct.h"
#include "../headers/util.h"
#include "../headers/first_pass.h"

int DC;
int IC;
int SDC;
int SIC;
int error_detect;

void add_to_ent_file(char *filename, const char *label, int address) {
    char *ent_filename;
    FILE *ent_file;

    /* יצירת שם הקובץ עם סיומת .ent */
    ent_filename = (char *)malloc(strlen(filename) + 5); /* 4 תווים עבור ".ent" ועוד אחד ל-null terminator */
    if (!ent_filename) {
        print_internal_error(ERROR_CODE_1); /* Error allocating memory */
        return;
    }
    strcpy(ent_filename, filename);
    strcat(ent_filename, ".ent");

    ent_file = fopen(ent_filename, "a");  /* פתיחה במצב append, כך שניתן להוסיף ערכים נוספים */
    if (!ent_file) {
        print_internal_error(ERROR_CODE_2); /* Error opening file */
        free(ent_filename);
        return;
    }

    fprintf(ent_file, "%s %04d\n", label, address);

    fclose(ent_file);
    free(ent_filename);
}

void second_pass(char *filename, char *fileNameAm) {
    FILE *fileAM;
    char line[MAX_LINE_LENGTH];
    int lineC = 0;
    int errC = 0;
    char *label = NULL;
    char *instruction = NULL;
    char *operand = NULL;
    char *operand1 = NULL;
    char *operand2 = NULL;
    location amFile;
    char binaryOutput[100][MAX_BINARY_LENGTH];
    InstructionInfo info;
    Symbol *sym1;
    Symbol *sym2;

    fileAM = fopen(fileNameAm, "r");
    if (!fileAM) {
        print_internal_error(ERROR_CODE_2);
        return;
    }

    while (fgets(line, MAX_LINE_LENGTH, fileAM)) {
        lineC++;
        amFile.file_name = filename;
        amFile.line_num = lineC;

        operand1 = operand2 = NULL;

        /* דילוג על שורות ריקות ותגובות */
        if (*line == '\0' || *line == ';' || *line == '\n') {
            continue;
        }

        /* ניתוח השורה */
        analyze_line(line, &label, &instruction, &operand, &amFile, &errC);

        if (instruction == NULL) {
            continue;  /* דילוג על השורה אם אין הוראה */
        }
        if (operand && strcmp(instruction, ".data") != 0 && strcmp(instruction, ".string") != 0 && strcmp(instruction, ".entry") != 0 && strcmp(instruction, ".extern") != 0) {
            operand1 = strtok(operand, ",");
            operand2 = strtok(NULL, ",");
        } else {
            operand1 = operand;
        }

        if (strcmp(instruction, ".data") == 0 || strcmp(instruction, ".string") == 0 || strcmp(instruction, ".extern") == 0 || strcmp(instruction, ".entry") == 0) {
            continue;  /* דילוג על הוראות לא רלוונטיות */
        } else {
            info = instructionLength(instruction, &operand1, &operand2, amFile);
            if (info.length == -1) {
                errC++;
                continue;
            }

            /* בדיקה אם אופרנד כלשהו הוא תווית */
            if (operand1 != NULL && get_operand_type(operand1) == 1) {
                sym1 = find_symbol(operand1);
                if (sym1 != NULL) {
                    if (sym1->is_entry) {
                        add_to_ent_file(fileNameAm, sym1->label, IC + 100);
                    }
                    if (sym1->is_external) {
                        add_to_ext_file(fileNameAm, sym1->label, IC + 100);
                    }
                }
            }

            if (operand2 != NULL && get_operand_type(operand2) == 1) {
                sym2 = find_symbol(operand2);
                if (sym2 != NULL) {
                    if (sym2->is_entry) {
                        add_to_ent_file(fileNameAm, sym2->label, sym2->address);
                    }
                    if (sym2->is_external) {
                        add_to_ext_file(fileNameAm, sym2->label, IC + 100);
                    }
                }
            }

            if (encodeInstruction(instruction, operand1, operand2, IC, binaryOutput, amFile) >= 0) {
                unsigned short binaryCode = (unsigned short)strtol(binaryOutput[IC], NULL, 2);
                add_code_node(IC, binaryCode);
                IC += info.length;
            } else {
                print_external_error(22, amFile);
                errC++;
            }
        }
    }

    fclose(fileAM);

    /* יצירת קובצי הפלט לאחר המעבר השני */
    create_ext_file(filename, symbol_table);
    create_ob_file(filename, binaryOutput, SIC, SDC);
}

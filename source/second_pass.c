
#include "../headers/util.h"
#include "../headers/globaldefine.h"
#include "../headers/data_struct.h"
#include "../headers/first_pass.h"
#include "../headers/files_handler.h"

int DC;
int IC;
int SDC;
int SIC;
int error_detect;


void second_pass(char *filename, char *fileNameAm) {
    FILE *fileAM;
    char line[MAX_LINE_LENGTH];
    int lineC = 0;
    int errC = 0;
    int analysis_result;
    char *label = NULL;
    char *instruction = NULL;
    char *operand = NULL;
    char *operand1 = NULL;
    char *operand2 = NULL;
    char *trimmed_line;
    location amFile;
    InstructionInfo info;
    CodeNode *code_list = NULL; 


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
            continue;
        }
        if (operand && strcmp(instruction, ".data") != 0 && strcmp(instruction, ".string") != 0 &&
            strcmp(instruction, ".entry") != 0 && strcmp(instruction, ".extern") != 0) {
            operand1 = strtok(operand, ",");
            operand2 = strtok(NULL, ",");
        } else {
            operand1 = operand;
        }

        if (strcmp(instruction, ".data") == 0) {
            encode_data(operand, &DC, &IC, &code_list);
            continue;
        } else if (strcmp(instruction, ".string") == 0) {
            encode_string(operand, &DC, &IC, &code_list);
            continue;
        } else if (strcmp(instruction, ".extern") == 0 || strcmp(instruction, ".entry") == 0) {
            continue;
        } else {
            info = instructionLength(instruction, &operand1, &operand2, amFile);
            if (info.length == -1) {
                errC++;
                continue;
            }

            /* קידוד ההוראה והאופרנדים */
            if (encodeInstruction(fileNameAm, instruction, operand1, operand2, IC, &code_list, amFile, 1) >= 0) {
                IC += info.length;
            } else {
                print_external_error(22, amFile);
                errC++;
            }
        }
    }
    create_entry_file(filename,symbol_table);
    create_ob_file(filename, code_list, SIC, SDC);

    fclose(fileAM);
    free_linked_list(macroTable);
    free_symbol_table();
    free_CodeNode_list();

}






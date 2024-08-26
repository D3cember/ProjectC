#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "data_struct.h"
#include "errors.h"

typedef struct {
    int length;  /* אורך ההוראה */
    int operand_types[2];  /* סוגי המיעון של האופרנדים (עד שניים) */
} InstructionInfo;


int count_data_items(const char *data);
int instruction(const char *word);
void first_pass(char *filename);
void adjust_data_symbols(int base);
const char *instr[16];
void encode_data(const char *operands, int *DC, int *IC, CodeNode **code_list);
void encode_string(const char *operand, int *DC, int *IC, CodeNode **code_list);
InstructionInfo instructionLength(const char *instruction, char **operand1, char **operand2, location amFile);
int is_reserved_keyword(const char *label);
void handle_extern(const char *label,int address);
void handle_entry(const char *label, int address);
int is_valid_label(const char *label);
int encodeInstruction(const char *filename, const char *instruction, char *operand1, char *operand2, int IC, CodeNode **code_list, location amFile, int is_second_pass);
void encode_operand_value(const char *filename, const char *operand, int operandType, int isSource, int IC, int *symbolFlag, CodeNode *code_list, int is_second_pass);
void int_to_binary(int value, int num_bits, char *binaryOutput);

#endif /*FIRST_PASS_H*/

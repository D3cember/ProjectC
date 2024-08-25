#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "errors.h"

#define MAX_BINARY_LENGTH 16

typedef struct {
    int length;  /* אורך ההוראה */
    int operand_types[2];  /* סוגי המיעון של האופרנדים (עד שניים) */
} InstructionInfo;


int count_data_items(const char *data);
int instruction(const char *word);
void first_pass(char *filename);
void adjust_data_symbols(int base);
const char *instr[16];
void encode_data(const char *operand , int *DC, char binaryOutput[][16], int *IC);
void encode_string(const char *operand, int *DC, char binaryOutput[][16], int *IC);
InstructionInfo instructionLength(const char *instruction, char **operand1, char **operand2, location amFile);
int is_reserved_keyword(const char *label);
void handle_extern(const char *label);
void handle_entry(const char *label);
int is_valid_label(const char *label);
int encodeInstruction(const char *instruction, char *operand1, char *operand2, int IC, char binaryOutput[][16], location amFile);
void encode_operand_value(const char *operand, int operandType, int isSource, char *binaryOutput, int *symbolFlag);
void int_to_binary(int value, int num_bits, char *binaryOutput);

#endif /*FIRST_PASS_H*/


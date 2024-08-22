#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "data_struct.h"
#include "errors.h"

int count_data_items(const char *data);
int instruction(const char *word);
void first_pass(char *filename);
void adjust_data_symbols(int base);
const char *instr[16];
void encode_data(const char *operand , int *DC);
void encode_string(const char *operand, int *DC);
int instructionLength(const char *instruction, const char *operand1, const char *operand2,location amFile);
int is_reserved_keyword(const char *label);
void handle_extern(const char *label); 
void handle_entry(const char *label);
int is_valid_label(const char *label);
int encoded_instruction(const char *operand1 , const char *operand2,const char *instruction);



#endif /*FIRST_PASS_H*/


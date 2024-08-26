#ifndef PROJECTC_UTIL_H
#define PROJECTC_UTIL_H

#include "data_struct.h"
#include "errors.h"

typedef enum {
    MOV,
    CMP,
    ADD,
    SUB,
    LEA,
    CLR,
    NOT,
    INC,
    DEC,
    JMP,
    BNE,
    JSR,
    RED,
    PRN,
    RTS,
    STOP,
    UNKNOWN
} InstructionType;

typedef struct {
    const char *name;
    InstructionType type;
    int opcode;
} InstructionMap;


extern const char *reserved_keywords[MAX_KEYWORDS];
char *Macro_name(const char* line);
int is_macro_call(const char *line);
int check_macro_declaration_format(const char *line);
char *format_operands(char *operand_str);
int get_operand_type(const char *operand);
int analyze_line(char *line, char **label, char **instruction, char **operand, location *amFile, int *errC);
InstructionType get_instruction_type(const char *instruction);
int get_opcode(const char *instruction);
int set_operand_bits(int operandType, int isSource);
Symbol* find_symbol(const char *label);
int is_register(int operand_type, const char *operand);




#endif

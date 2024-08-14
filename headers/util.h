#ifndef PROJECTC_UTIL_H
#define PROJECTC_UTIL_H



#include "data_struct.h"

extern const char *reserved_keywords[MAX_KEYWORDS];
char *Macro_name(const char* line);
int is_macro_call(const char *line);
int check_macro_declaration_format(const char *line);
char *format_operands(char *operand_str);


#endif

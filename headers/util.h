#ifndef PROJECTC_UTIL_H
#define PROJECTC_UTIL_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "data_struct.h"

extern const char *reserved_keywords[20];
char* trim_whitespace(char* str);
char *Macro_name(const char* line);
int is_macro_call(const char *line);


#endif
